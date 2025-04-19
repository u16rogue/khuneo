const std = @import("std");
const Allocator = std.mem.Allocator;

pub const StackIndex = i32;
pub const StackError = error { OutOfBound } || Allocator.Error;

/// Stack
///
/// A data structure that allows you to push and pop
/// values of type `T` in "first in last out" order / operation
pub fn Stack(T: type) type {
    return struct {
        const Self = @This();

        /// Stack Iterator
        pub const Iterator = struct {
            _instance: *Self,
            _current: usize,

            /// Obtains the next item's pointer in the stack
            /// Returns `null` when all items have been iterated
            pub fn next(iter: *Iterator) ?*T {
                if (iter._current >= iter._instance._count) {
                    return null;
                }
                iter._current += 1;
                return &iter._instance._items[iter._instance._count - iter._current];
            }
        };

        _allocator: Allocator,
        _items: []T,
        _count: usize,
        _capacity: usize,

        /// Initializes a Stack instance
        pub fn init(allocator: Allocator) Self {
            return Self {
                ._allocator = allocator,
                ._items = undefined,
                ._count = 0,
                ._capacity = 0,
            };
        }

        /// Deinitialize the stack instance.
        /// Accessing / Using the instance after
        /// is undefined behavior.
        pub fn deinit(self: *Self) void {
            if (self._capacity != 0) {
                self._allocator.free(self._items);
            }
        }

        /// Pushes the value of type `T` on top
        /// of the stack
        pub fn push(self: *Self, value: T) StackError!void {
            if (self._needToGrow()) {
                try self._grow(1);
            }
            self._items[self._count] = value;
            self._count += 1;
        }

        /// Pops the value of type `T` from the
        /// top of the stack and returns it.
        /// Results to `null` if there is no
        /// value to pop
        pub fn pop(self: *Self) ?T {
            if (self.peek()) |value| {
                self._count -= 1;
                return value.*;
            } else {
                return null;
            }
        }

        /// Peek at the top value of the stack by
        /// obtaining a pointer to it. Returns `null`
        /// if there is no value to peek at
        pub fn peek(self: *Self) ?*T {
            if (self._count == 0) {
                return null;
            }
            return &self._items[self._count - 1];
        }

        /// Indexed peek into the stack starting from the top 
        pub fn peekAt(self: *Self, index: StackIndex) StackError!*T {
            if ((index >= 0 and index >= self._count)
            or  ((@abs(index) -| 1) >= self._count)
            ){
                return StackError.OutOfBound;
            }

            return self.peekAtUnsafe(index);
        }

        /// Indexed peek into the stack starting from the top without
        /// bound check and assertion
        pub fn peekAtUnsafe(self: *Self, index: StackIndex) *T {
            if (index < 0) {
                // * This can be bit masked to remove the least (1) and
                //   most (negative bit) significant bits
                // * Guaranteed to be less than zero so wrap around should be impossible
                return &self._items[@abs(index) - 1]; 
            } else {
                // Guaranteed positive so illegal behavior should be impossible
                return &self._items[self._count - 1 - @as(usize, @intCast(index))];
            }
        }

        /// Obtains the stack index of the peek result relative to the
        /// bottom of the stack
        pub fn indexOfPeekFromBottom(self: *Self) ?StackIndex {
            if (self._count == 0) {
                return null;
            }
            return -@as(StackIndex, @intCast(self._count));
        }

        /// Returns the number of items in the stack
        pub fn count(self: *Self) usize {
            return self._count;
        }

        /// Creates an iterator for the stack values
        pub fn iterator(self: *Self) Iterator {
            return Iterator {
                ._instance = self,
                ._current = 0,
            };
        }

        /// INTERNAL: Checks if the items buffer needs to
        /// be expanded
        fn _needToGrow(self: *Self) bool {
            return self._count >= self._capacity;
        }

        /// INTERNAL: Expands the stack capacity by nslot
        fn _grow(self: *Self, nslot: usize) !void {
            const new_capacity = self._capacity + nslot;
            const new_items = try self._allocator.alloc(T, new_capacity);
            errdefer self._allocator.free(new_items);

            // A capacity of 0 means we haven't allocated `_items` yet
            // therefore there is no need to copy over as `_count` can be
            // zero when all items are popped
            if (self._capacity != 0) {
                for (0..self._count) |i| {
                    new_items[i] = self._items[i];
                }
                self._allocator.free(self._items);
            }
            self._items = new_items;
            self._capacity = new_capacity;
        }

        /// INTERNAL: Shrinks the stack capacity to its maximum
        /// count
        fn _shrink(self: *Self) StackError!void {
            _ = self;
            @compileError("not implemented");
        }
    };
}

test "Stack" {
    const testing = std.testing;
    var stack = Stack(u8).init(testing.allocator);
    defer stack.deinit();

    // An empty stack must result to null
    try testing.expect(stack.peek() == null);
    try testing.expect(stack.pop() == null);
    try testing.expect(stack.peekAt(0) == StackError.OutOfBound);
    try testing.expect(stack.peekAt(-1) == StackError.OutOfBound);
    try testing.expect(stack.indexOfPeekFromBottom() == null);

    try stack.push(1);
    try testing.expect(stack.indexOfPeekFromBottom() == -1);

    try stack.push(2);
    try testing.expect(stack.indexOfPeekFromBottom() == -2);

    try testing.expect((try stack.peekAt(0)).* == 2);
    try testing.expect((try stack.peekAt(1)).* == 1);
    try testing.expect(stack.peekAt(2) == StackError.OutOfBound);
    try testing.expect((try stack.peekAt(-1)).* == 1);
    try testing.expect((try stack.peekAt(-2)).* == 2);
    try testing.expect(stack.peekAt(-3) == StackError.OutOfBound);

    try testing.expect(stack.peek().?.* == 2);
    try testing.expect(stack.pop().? == 2);
    try testing.expect(stack.peek().?.* == 1);
    try testing.expect(stack.pop().? == 1);

    try stack.push(3);
    try stack.push(4);

    var iterator = stack.iterator();
    try testing.expect(iterator.next().?.* == 4);
    try testing.expect(iterator.next().?.* == 3);
    try testing.expect(iterator.next() == null);
}

const StringHashMapError = error {} || Allocator.Error;

/// String Hashmap
///
/// Hashes a string that acts as a key to a value.
/// The string key is allocated and managed by the instance.
///
/// TODO: actually implement as a hashmap
pub fn StringHashMap(T: type) type {
    return struct {
        const Self = @This();

        pub const Iterator = struct {
            _instance: *Self,
            _current: usize,

            /// Obtain the next key value pair within the hash map.
            /// Returns null when iteration has been exhausted. The
            /// reference returned by the iterator points to the internal
            /// buffer that may be invalidated on subsequent data operations.
            /// Reference is meant for direct read and writes which should not
            /// be stored in the context of persistence.
            pub fn next(self: *Iterator) ?*KeyValue {
                if (self._current >= self._instance._count) {
                    return null;
                }
                const kv = &self._instance._kv[self._current];
                self._current += 1;
                return kv;
            }
        };

        const KeyValue = struct {
            key: []u8,
            value: T,
        };

        _allocator: Allocator,
        _kv: []KeyValue,
        _count: usize,
        _capacity: usize,

        /// Initializes a string hashmap instance
        pub fn init(allocator: Allocator) Self {
            return Self {
                ._allocator = allocator,
                ._kv = undefined,
                ._count = 0,
                ._capacity = 0,
            };
        }

        /// Deinitializes the string hashmap instance
        pub fn deinit(self: *Self) void {
            if (self._capacity == 0) {
                return;
            }

            // We use count and not capacity as capacity range may
            // contain an undefined `key` while `count` ensures a
            // range with actual allocated keys to free as it ties
            // to the inserted kv pairs
            for (0..self._count) |i| {
                self._allocator.free(self._kv[i].key);
            }

            self._allocator.free(self._kv);
        }

        /// Obtains a pointer to the value provided by a key
        /// Returns `null` if the key is not found.
        /// The pointer returned points to the internal buffer
        /// that may get invalidated on data operations. Only
        /// use this for the purpose of directly modifying the value
        /// or a copy would be too expensive
        pub fn getPtr(self: *Self, key: []const u8) ?*T {
            if (self._count == 0) {
                return null;
            }

            for (0..self._count) |i| {
                const kv = &self._kv[i];
                if (std.mem.eql(u8, kv.key, key)) {
                    return &kv.value;
                }
            }

            return null;
        }

        /// Obtains the value stored by a key
        pub fn get(self: *Self, key: []const u8) ?T {
            if (self.getPtr(key)) |value| {
                return value.*;
            } else {
                return null;
            }
        }

        /// Checks if a key exists
        pub fn has(self: *Self, key: []const u8) bool {
            if (self._count == 0) {
                 return false;
            }

            for (0..self._count) |i| {
                if (std.mem.eql(u8, self._kv[i].key, key)) {
                    return true;
                }
            }

            return false;
        }

        /// Set a key to a value. If a key already exists the value
        /// is overwritten if it doesn't it is then created.
        ///
        /// `key` is duplicated and managed by the instance. Unlike `std.StringHashMap`.
        pub fn set(self: *Self, key: []const u8, value: T) StringHashMapError!void {
            if (self._count >= self._capacity) {
                const new_capacity = self._capacity + 1;
                const new_kv = try self._allocator.alloc(KeyValue, new_capacity);
                errdefer self._allocator.free(new_kv);
                if (self._capacity != 0) {
                    for (0..self._count) |i| {
                        new_kv[i] = self._kv[i];
                    }
                    self._allocator.free(self._kv);
                }
                self._kv = new_kv;
                self._capacity = new_capacity;
            }

            for (0..self._count) |i| {
                const kv = &self._kv[i];
                if (std.mem.eql(u8, kv.key, key)) {
                    kv.value = value;
                    return;
                }
            }

            const new_key = try self._allocator.dupe(u8, key);
            errdefer self._allocator.free(new_key);

            self._kv[self._count] = KeyValue {
                .key = new_key,
                .value = value,
            };
            self._count += 1;
            return;
        }

        /// Creates an iterator for each key value pair
        pub fn iterator(self: *Self) Iterator {
            return Iterator {
                ._instance = self,
                ._current = 0,
            };
        }
    };
}

test "StringHashMap" {
    const testing = std.testing;

    var shm = StringHashMap(u8).init(testing.allocator);
    defer shm.deinit();

    try testing.expect(shm.get("one") == null);
    try testing.expect(shm.getPtr("one") == null);

    try shm.set("one", 1);
    try testing.expect(shm.get("one").? == 1);

    try shm.set("two", 2);
    try testing.expect(shm.get("two").? == 2);

    shm.getPtr("two").?.* = 4;
    try testing.expect(shm.get("two").? == 4);
    try testing.expect(shm.getPtr("two").?.* == 4);

    try testing.expect(false);
}
