#include <khuneo/compiler/ast.hpp>

auto khuneo::ast::node::link_forward(node * n) -> node *
{
	n->prev = this;
	next = n;

	return this;
}