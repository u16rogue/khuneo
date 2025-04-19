const std = @import("std");

pub const Token = union (enum) {
    exhausted: void,
    comment: []const u8,
    identifier: []const u8,
    number: []const u8,
    string: []const u8,
    sym_colon: void,
    sym_colon_assign: void,
    sym_assign: void,
    sym_eql: void,
    sym_semi_colon: void,
    sym_at: void,
    sym_paren_open: void,
    sym_paren_close: void,
    sym_dot: void,
    whitespace: usize,

    pub fn length(self: *const Token) usize {
        return switch (self.*) {
            .exhausted => 0,
            .identifier => self.identifier.len,
            .comment => self.comment.len,
            .number => self.number.len,
            .whitespace => self.whitespace,
            .string => self.string.len,
            .sym_colon => 1,
            .sym_colon_assign => 2,
            .sym_assign => 1,
            .sym_eql => 2,
            .sym_semi_colon => 1,
            .sym_at => 1,
            .sym_paren_open, .sym_paren_close => 1,
            .sym_dot => 1,
        };
    }
};

pub const Tokenizer = struct {
    pub const Error = error { InvalidCharacter, OutOfBound, SyntaxError };

    _source: []const u8,
    _index: usize,

    /// Initializes a tokenizer state for tokenizing a slice that
    /// represents a source code. The slice is only referenced
    /// in this instance therefore the caller is responsible
    /// for the slices' lifetime.
    pub fn init(source: []const u8) Tokenizer {
        return Tokenizer {
            ._source = source,
            ._index = 0,
        };
    }

    /// De-initialize a tokenizer instance
    pub fn deinit(self: *Tokenizer) void {
        _ = self;
    }

    fn partialMatch(indexed_source: []const u8, match: []const u8) bool {
        return std.mem.eql(u8, indexed_source[0..match.len], match);
    }

    /// Obtains the next valid token within the source and returns it as
    /// a slice of the internal source buffer reference without
    /// forwarding / moving the internal state. The slice points to the
    /// internal reference source buffer that is provided by the caller of `create`.
    pub fn peek(self: *Tokenizer) Error!Token {
        if (self._index >= self._source.len) {
            return .exhausted;
        }

        const indexed_source = self._source[self._index..];
        var match_len: usize = 0;
        switch (indexed_source[0]) {
            '#' => {
                match_len += 1;
                for (indexed_source[1..]) |character| {
                    if (character == '\n') break;
                    match_len += 1;
                }
                return .{ .comment = indexed_source[0..match_len] };
            },
            'A'...'Z', 'a'...'z', '_', '$' => {
                match_len += 1;
                for (indexed_source[1..]) |character| {
                    switch (character) {
                        'A'...'Z', 'a'...'z', '_', '$', '0'...'9' => match_len += 1,
                        else => break,
                    }
                }
                return .{ .identifier = indexed_source[0..match_len] };
            },
            '0'...'9' => {
                match_len += 1;
                for (indexed_source[1..]) |character| {
                    switch (character) {
                        '0'...'9' => match_len += 1,
                        else => break,
                    }
                }
                return .{ .number = indexed_source[0..match_len] };
            },
            ' ', '\n', '\t', '\r' => {
                match_len += 1;
                for (indexed_source[1..]) |character| {
                    switch (character) {
                        ' ', '\n', '\t', '\r' => match_len += 1,
                        else => break,
                    }
                }
                return .{ .whitespace = match_len };
            },
            ':' => {
                match_len += 1;
                if (partialMatch(indexed_source[1..], "=")) return .sym_colon_assign;
                return .sym_colon;
            },
            '=' => {
                match_len += 1;
                if (partialMatch(indexed_source[1..], "=")) return .sym_eql;
                return .sym_assign;
            },
            ';' => {
                match_len += 1;
                return .sym_semi_colon;
            },
            '@' => {
                match_len += 1;
                return .sym_at;
            },
            '(' => {
                match_len += 1;
                return .sym_paren_open;
            },
            ')' => {
                match_len += 1;
                return .sym_paren_open;
            },
            '.' => {
                match_len += 1;
                return .sym_dot;
            },
            '\'' => {
                match_len += 1;
                for (indexed_source[1..]) |character| {
                    match_len += 1;
                    switch (character) {
                        '\'' => return .{ .string = indexed_source[0..match_len] },
                        '\n' => return Error.SyntaxError,
                        else => {},
                    }
                }
                return Error.SyntaxError;
            },
            else => {
                std.log.err("invalid character: '{s}'", .{ indexed_source[0..1] });
                return Error.InvalidCharacter;
            },
        }
    }

    /// Obtains the next valid token within the source and
    /// returns it as a slice of the internal source buffer reference.
    /// The slice points to the internal reference source buffer that is
    /// provided by the caller of `create`.
    pub fn next(self: *Tokenizer) !Token {
        const result = try self.peek();
        self._index +|= result.length();
        return result;
    }
};
