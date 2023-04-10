#pragma once

#include <khuneo/core/defs.hpp>

namespace khuneo::cont::details {
}

namespace khuneo::cont {

template <typename T, typename impl>
class flat_tree {
  using self_t = flat_tree;
  using sz_t   = khuneo::u32;

public:
  struct node_t {
    T value;
  private:
    khuneo::u8 left_offset;
    khuneo::u8 right_offset;
  };

public:
  static auto construct(self_t * self) -> bool {
    self->data       = nullptr;
    self->real_count = 0;
    self->used_count = 0;
    return true;
  }

  static auto destroy(self_t * self) -> bool {
  }

  static auto left_node(self_t * self, node_t * node) -> node_t * {
  }

  static auto right_node(self_t * self, node_t * node) -> node_t * {
  }

  static auto append_left(self_t * self, node_t * node) -> node_t * {
  }

  static auto append_right(self_t * self, node_t * node) -> node_t * {
  }

private:
  static auto block_delta(node_t * a, node_t * b) -> khuneo::u8 {
  }

  static auto acquire_block() -> node_t * {
  }

  static auto count_to_bytes(sz_t sz) -> khuneo::u32 {
    return sizeof(node_t) * sz;
  }

  static auto grow(self_t * self, sz_t rel_sz) -> bool {
  }

private:
  node_t * data;
  sz_t     real_count; // Number of elements actually allocated (may allocate more for optimization of rapid resizing)
  sz_t     used_count; // Number of elements stored
};

}
