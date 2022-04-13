#pragma once

namespace khuneo::ast
{
	struct node
	{
		node * next;
		node * prev;

		node * parent;
		node * child;

		unsigned long tok_id;
		const char *  tok_name;

		int line;
		int column;

		const char * start;
		const char * end;
		
		/*
		* Links the node [n] to this node
		* as the next node. This returns
		* a pointer to itself (this).
		*/
		auto link_forward(node * n) -> node *;
	};
}