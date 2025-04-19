const std = @import("std");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer if (gpa.deinit() == .leak) @panic("memory leak detected");
    const allocator = gpa.allocator();
    try @import("host/cli.zig").main(allocator);
}
