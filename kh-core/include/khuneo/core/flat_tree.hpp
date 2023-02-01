#pragma once

#include <khuneo/core/defs.hpp>

namespace khuneo::cont::details {
}

namespace khuneo::cont {

template <typename T, typename impl>
struct flat_tree_node {
  T value;
  khuneo::u8 left_offset;
  khuneo::u8 right_offset;
};

template <typename T, typename impl>
class flat_tree {
};

}
