#include <khuneo/compiler/ast.hpp>

auto khuneo::ast::node::link_next(node * n) -> node *
{
	n->prev = this;
	next = n;

	return this;
}

auto khuneo::ast::node::link_child(node * n) -> node *
{
	n->parent = this;
	child = n;

	return this;
}