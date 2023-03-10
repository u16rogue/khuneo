#pragma once

#include <khuneo/core/metapp.hpp>
#include <khuneo/compiler/lexer.hpp>

namespace khuneo::compiler::parser::details {

struct default_parser_impl {
  // using _extend = typename metapp::type_if<metapp::is_t_invalid<extend>::value, default_parser_impl, extend>::type;
  using lexer_impl = khuneo::compiler::lexer::details::default_lexer_impl<>;

  using token_node       = typename lexer::token_node<lexer_impl>;
  using token_container  = typename lexer_impl::container;

  using ast_container = void;
};

} // namespace khuneo::compiler::parser::details

namespace khuneo::compiler::parser {

template <typename impl>
struct run_info {
  typename impl::token_container * tokens;
  typename impl::ast_container   * ast;
};

template <typename impl>
auto run(run_info<impl> * i) -> bool {
  return false;
}

} // namespace khuneo::compiler::parser
