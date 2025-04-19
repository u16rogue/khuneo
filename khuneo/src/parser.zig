const std = @import("std");
const lexer = @import("lexer.zig");

const Allocator = std.mem.Allocator;

pub const AstNode = union (enum) {
    define: struct {
        name: []const u8,
        initializer: ?*AstNode,
    },
    number: []const u8,
};

pub const Error = error { NotImplemented } || lexer.Tokenizer.Error || Allocator.Error;

fn tokenNextFiltered(tokenizer: *lexer.Tokenizer) lexer.Tokenizer.Error!lexer.Token {
    while (tokenizer.next()) |token| {
        switch (token) {
            .whitespace => continue,
            else => return token,
        }
    } else |err| {
        return err;
    }
}

/// Creates and initializes an AST node
fn createAstNode(allocator: Allocator, value: AstNode) Allocator.Error!*AstNode {
    const node = try allocator.create(AstNode);
    node.* = value;
    return node;
}

/// Recursively destroys an AST
pub fn destroyAstNodesRecursively(allocator: Allocator, node: *AstNode) void {
    switch (node.*) {
        .define => |define| {
            allocator.free(define.name);
            if (define.initializer) |initializer| {
                destroyAstNodesRecursively(allocator, initializer);
            }
        },
        .number => |number| {
            allocator.free(number);
        }
    }
    allocator.destroy(node);
}

/// Parses a tokenizer stream and generates an AST of the parsed tokens
/// Nodes are allocated with the provided allocator use `destroyAstNodesRecursively` to
/// recursively destroy nodes.
pub fn parse(allocator: Allocator, tokenizer: *lexer.Tokenizer) Error!?*AstNode {
    // Only build AST nodes AFTER allocating everything. Let the errdefer handle the deallocation
    // once we're sure that there are no errors that could occure only then we build the branches
    while (tokenNextFiltered(tokenizer)) |token| {
        switch (token) {
            .identifier => |identifier| {
                if (std.mem.eql(u8, identifier, "var")) {
                    const token_var_name = try tokenNextFiltered(tokenizer);
                    if (token_var_name != .identifier) {
                        return Error.SyntaxError;
                    }

                    switch (try tokenNextFiltered(tokenizer)) {
                        .sym_semi_colon => {
                            return try createAstNode(allocator, AstNode { .define = .{ .name = try allocator.dupe(u8, token_var_name.identifier), .initializer = null } });
                        },
                        .sym_assign => {
                            const managed_var_name = try allocator.dupe(u8, token_var_name.identifier);
                            errdefer allocator.free(managed_var_name);
                            return try createAstNode(allocator, AstNode { .define = .{
                                .name = managed_var_name,
                                .initializer = try parse(allocator, tokenizer),
                            } });
                        },
                        else => {
                            return Error.SyntaxError;
                        },
                    }
                } else {
                    return Error.NotImplemented;
                }
            },
            .number => |raw| {
                switch (try tokenNextFiltered(tokenizer)) {
                    .sym_semi_colon => { return createAstNode(allocator, AstNode { .number = try allocator.dupe(u8, raw) }); },
                    else => return Error.SyntaxError,
                }
            },
            .exhausted => {
                return null;
            },
            else => {
                return Error.SyntaxError;
            },
        }
        return Error.SyntaxError;
    } else |err| {
        return err;
    }
    return Error.NotImplemented;
}
