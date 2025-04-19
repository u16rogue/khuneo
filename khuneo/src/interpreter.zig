const std = @import("std");

const parser = @import("parser.zig");
const datastructures = @import("datastructures.zig");
const Number = @import("Number.zig");

const Allocator = std.mem.Allocator;

const Symbol = struct {
    value_ref: usize,
};

const StackValue = struct {
    value_ref: usize,
};

const Value = struct {
    pub const Error = error {} || Allocator.Error;
    pub const String = []u8;

    _kind: union (enum) {
        nil: void,
        string: String,
        number: Number,
    },
    _cow: enum {
        clean,
        dirty,
    },

    pub fn initNil() Value {
        return Value {
            ._kind = .nil,
            ._cow = .clean,
        };
    }

    pub fn initString(allocator: Allocator, string: []const u8) Error!Value {
        const alloc_string = try allocator.dupe(u8, string);
        errdefer allocator.free(alloc_string);
        return Value {
            ._kind = .{ .string = alloc_string },
            ._cow = .clean,
        };
    }

    pub fn initNumber(number: Number) Error!Value {
        return Value {
            ._kind = .{ .number = number },
            ._cow = .clean,
        };
    }

    pub fn deinit(self: *Value, allocator: Allocator) void {
        switch (self._kind) {
            .string => |string| {
                allocator.free(string);
            },
            .number => |*number| {
                number.deinit(allocator);
            },
            .nil => {
                // do nothing
            },
        }
    }

    pub fn toString(self: *Value, allocator: Allocator) std.fmt.AllocPrintError![]const u8 {
        switch (self._kind) {
            .nil => {
                return try allocator.dupe(u8, "<nil>");
            },
            .string => |string| {
                return try allocator.dupe(u8, string);
            },
            .number => |*number| {
                return try number.toString(allocator);
            }
        }
    }
};

/// Isolated Context of an interpreter execution
pub const Context = struct {
    const Symbols = datastructures.StringHashMap(Symbol);
    const Stack = datastructures.Stack(StackValue);
    const Values = std.ArrayList(Value);

    pub const Error = error { Invalid, SymbolRedefinition } || Allocator.Error || datastructures.StackError;
    pub const ValueRefId = usize;

    _allocator: Allocator,
    _symbols: Symbols,
    _values: Values,
    _stack: Stack,

    pub fn init(allocator: Allocator) Context {
        const ctx = Context {
            ._allocator = allocator,
            ._symbols = Symbols.init(allocator),
            ._values = Values.init(allocator),
            ._stack = Stack.init(allocator),
        };
        return ctx;
    }

    pub fn deinit(self: *Context) Error!void {
        self._symbols.deinit();
        self._stack.deinit();

        for (self._values.items) |*value| {
            value.deinit(self._allocator);
        }
        self._values.deinit();
    }

    pub fn symbolsAdd(self: *Context, name: []const u8, vri: ValueRefId) Error!void {
        if (self._symbols.has(name)) {
            return Error.SymbolRedefinition;
        }
        try self._symbols.set(name, Symbol { .value_ref = vri });
    }

    pub fn symbolsHas(self: *Context, name: []const u8) bool {
        return self._symbols.has(name);
    }

    pub fn stackPush(self: *Context, value: StackValue) Error!void {
        try self._stack.push(value);
    }

    pub fn stackPop(self: *Context) ?StackValue {
        return self._stack.pop();
    }

    pub fn valueAdd(self: *Context, value: Value) Error!ValueRefId {
        const id = self._values.items.len;
        try self._values.append(value);
        return id;
    }

    pub fn valueRefGet(self: *Context, id: ValueRefId) ?*Value {
        return &self._values.items[id];
    }
};

pub const ErrorInterpreter = error { NotImplemented, NoValue } || Context.Error || Number.Error;
pub fn executeWithAst(ctx: *Context, ast: *const parser.AstNode) ErrorInterpreter!void {
    switch (ast.*) {
        .define => |define| {
            if (ctx.symbolsHas(define.name)) {
                return ErrorInterpreter.SymbolRedefinition;
            }

            if (define.initializer) |initializer| {
                try executeWithAst(ctx, initializer);
            } else {
                const ref_id = try ctx.valueAdd(Value.initNil());
                try ctx.stackPush(StackValue { .value_ref = ref_id });
            }

            if (ctx.stackPop()) |refid| {
                try ctx.symbolsAdd(define.name, refid.value_ref);
            } else {
                return ErrorInterpreter.NoValue;
            }
        },
        .number => |number| {
            const num_obj = try Number.initFromString(number);
            const num_val = try Value.initNumber(num_obj);
            const num_ref = try ctx.valueAdd(num_val);
            try ctx.stackPush(StackValue { .value_ref = num_ref });
        },
    }
}
