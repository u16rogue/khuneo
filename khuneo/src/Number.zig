const std = @import("std");
const Number = @This();
const Allocator = std.mem.Allocator;
pub const Error = error {} || std.fmt.ParseIntError;

/// Represents the actual true value of the `Number`
_underlying: union(enum) {
    signed64: i64,
    //managed: []const u8,
},

/// Initialize from a string that represents a number
pub fn initFromString(buf: []const u8) Error!Number {
    return Number {
        ._underlying = .{
            .signed64 = try std.fmt.parseInt(i64, buf, 10),
        },
    };
}

/// Initialize directly from a signed 64 bit number
pub fn initFromSigned64(num: i64) Error!Number {
    return Number {
        ._underlying = .{ .signed64 = num }
    };
}

/// Deinitializes the number instance
pub fn deinit(self: *Number, allocator: Allocator) void {
    _ = .{ allocator, self };
}

/// Perform an arithmetic addition between two `Number`s
pub fn addNumber(self: *Number, other: *Number, allocator: Allocator) Number {
    _ = allocator;
    switch (self._underlying) {
        .signed64 => |self_num| {
            switch (other._underlying) {
                .signed64 => |other_num| {
                    return Number.initFromSigned64(self_num + other_num);
                },
            }
        },
    }
    return Number.initFromSigned64(self._underlying.signed64 + other._underlying.signed64);
}

/// Convert the current `Number` into its string representation that is
/// allocated into a buffer that the caller owns
pub fn toString(self: *Number, allocator: std.mem.Allocator) std.fmt.AllocPrintError![]const u8 {
    switch (self._underlying) {
        .signed64 => |num| {
            return try std.fmt.allocPrint(allocator, "{d}", .{ num });
        },
    }
}
