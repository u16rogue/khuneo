const std = @import("std");
const khuneo = @import("khuneo");

const Error = error { NotImplemented, Invalid, Parameters } || std.mem.Allocator.Error;

pub fn main(allocator: std.mem.Allocator) !void {
    const stdout = std.io.getStdOut().writer();

    var args = try std.process.argsWithAllocator(allocator);
    defer args.deinit();

    if (!args.skip()) {
        return Error.Invalid;
    }

    if (args.next()) |mode_prompt| {
        if (std.mem.eql(u8, mode_prompt, "run")) {
            var source_buffer: []u8 = undefined;
            if (args.next()) |arg_file| {
                source_buffer = try std.fs.cwd().readFileAlloc(allocator, arg_file, 4096);
            } else {
                std.log.err("no file parameter provided", .{});
                return Error.Parameters;
            }
            defer allocator.free(source_buffer);

            var tokenizer = khuneo.lexer.Tokenizer.init(source_buffer);
            defer tokenizer.deinit();

            var context = khuneo.interpreter.Context.init(allocator);
            defer context.deinit() catch @panic("failed to deinitialize interpreter context");

            khuneo.evalWithContextAndTokenizer(allocator, &context, &tokenizer) catch |err| {
                var line: usize = 1;
                var column: usize = 0;
                for (tokenizer._source, 0..) |value, current_index| {
                    column += 1;
                    if (value == '\n') {
                        line += 1;
                        column = 1;
                    }
                    if (current_index == tokenizer._index) {
                        break;
                    }
                }
                std.log.err("{s} occured on line {d} column {d} index {d}", .{ @errorName(err), line, column, tokenizer._index });
                std.log.err("last index value: {s}", .{ tokenizer._source[tokenizer._index..tokenizer._index+1] });
                return err;
            };

            if (std.log.logEnabled(.debug, .default)) {
                var iter_symbols = context._symbols.iterator();
                while (iter_symbols.next()) |symbol| {
                    const value = try context.valueRefGet(symbol.value.value_ref).?.toString(context._allocator);
                    defer context._allocator.free(value);
                    std.log.debug("symbol: {s} => {s}", .{ symbol.key, value });
                }
            }
        } else if (std.mem.eql(u8, mode_prompt, "query")) {
            if (args.next()) |info| {
                if (std.mem.eql(u8, info, "version")) {
                    try stdout.print("0.0.1", .{});
                } else {
                    std.log.err("unknown query parameter '{s}'", .{ info });
                    return Error.Parameters;
                }
            } else {
                std.log.err("invalid query parameter", .{});
                return Error.Parameters;
            }
        } else if (std.mem.eql(u8, mode_prompt, "help")) {
            try stdout.print(
                \\Platform and Environment for the Khuneo Runtime
                \\Usage: pkr [mode]
                \\  run <source file> - Run the specified file.
                \\  pkg               -
                \\  update [branch]   - Perform a self update.
                \\  query [info]      - Query information.
                \\  test              -
                \\  help              - Show the help dialog.
                ,
                .{}
            );
        } else {
            std.log.err("invalid mode parameter provided \"{s}\" try using help", .{ mode_prompt });
            return Error.Parameters;
        }
    } else {
        std.log.err("missing runtime mode parameter", .{});
        return Error.Parameters;
    }

    return;
}
