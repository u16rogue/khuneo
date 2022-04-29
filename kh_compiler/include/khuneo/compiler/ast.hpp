#pragma once

namespace khuneo::ast
{

	/*
	* Notes:
	*	> Possibly just use an integer to determine
	*	  the size of the entire block instead of a
	*     start and end pointer, this way we save
	*     something like 4 bytes depending on the platform
	*/

	struct node
	{
		// Indicates whether this node is occupied or not
		bool occupied;

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
		auto link_next(node * n) -> node *;

		/*
		* Links the node [n] to this node
		* as a child node. This returns a
		* pointer to itself.
		*/
		auto link_child(node * n) -> node *;
	};
}