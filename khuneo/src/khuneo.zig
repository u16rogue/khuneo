const std = @import("std");

pub const lexer          = @import("lexer.zig");
pub const parser         = @import("parser.zig");
pub const khvm           = @import("khvm.zig");
pub const interpreter    = @import("interpreter.zig");
pub const datastructures = @import("datastructures.zig");

pub fn evalWithContextAndTokenizer(allocator: std.mem.Allocator, ctx: *interpreter.Context, tokenizer: *lexer.Tokenizer) !void {
    while (try parser.parse(allocator, tokenizer)) |ast| {
        defer parser.destroyAstNodesRecursively(allocator, ast);
        try interpreter.executeWithAst(ctx, ast);
    }
}

pub fn eval(allocator: std.mem.Allocator, code_buffer: []const u8) !void {
    var tokenizer = lexer.Tokenizer.init(code_buffer);
    defer tokenizer.deinit();

    var context = interpreter.Context.init(allocator);
    defer context.deinit() catch @panic("");

    try evalWithContextAndTokenizer(allocator, &context, &tokenizer);
}

test {
    _ = .{ lexer, parser, khvm, interpreter, datastructures };
}
