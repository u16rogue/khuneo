#pragma once

#include <khuneo/compiler/lexer.hpp>

namespace khuneo::compiler::parser::details {

template <typename lexer_impl>
struct default_parser_impl {
  using token_node = typename lexer::token_node<lexer_impl>;
  using token_container  = typename lexer_impl::container;
};

} // namespace khuneo::compiler::parser::details

namespace khuneo::compiler::parser {

template <typename parser_impl>
auto run(typename parser_impl::token_container * tokens) -> bool {
  return false;
}

} // namespace khuneo::compiler::parser
