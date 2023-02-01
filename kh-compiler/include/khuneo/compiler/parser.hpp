#pragma once

#include <khuneo/compiler/lexer.hpp>

namespace khuneo::compiler::parser::details {

template <typename lexer_impl>
struct default_parser_impl {
  using token_node = typename lexer::token_node<lexer_impl>;
  using container  = typename lexer_impl::container;
};

} // namespace khuneo::compiler::parser::details

namespace khuneo::compiler::parser {

template <typename parser_impl>
auto run(khuneo::cont::contiguous_list<typename parser_impl::token_node, typename parser_impl::contiguous_list_impl> * tokens) -> bool {
  return false;
}

} // namespace khuneo::compiler::parser
