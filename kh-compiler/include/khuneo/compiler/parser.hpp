#pragma once

#include <khuneo/core/metapp.hpp>
#include <khuneo/compiler/lexer.hpp>

// Forward declarations
namespace khuneo::compiler::parser {
template <typename impl> struct run_info;
}

namespace khuneo::compiler::parser::details {

template <typename lexer_impl>
struct default_parser_impl {
  using token_node       = typename lexer::token_node<lexer_impl>;
  using token_container  = typename lexer_impl::container;

  using ast_container = void;
};

template <typename impl>
constexpr auto is_tok_end(run_info<impl> i) -> bool {
  return i->token_index >= decltype(i->tokens)::count(i->tokens);
}

template <typename impl>
auto dispatch_undefined(run_info<impl> i) -> bool {
}

} // namespace khuneo::compiler::parser::details

namespace khuneo::compiler::parser {

template <typename impl>
struct run_info {
  int                              token_index;
  typename impl::token_container * tokens;
  typename impl::ast_container   * ast;
};

template <typename impl>
auto run(run_info<impl> * i) -> bool {
  using tkcont = decltype(i->tokens);

  while (details::is_tok_end(i)) {

    ++i->token_index;
  }

  return true;
}

} // namespace khuneo::compiler::parser
