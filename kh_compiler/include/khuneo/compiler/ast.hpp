#pragma once

namespace khuneo::ast
{
	struct ast_node
	{
		ast_node * node_parent { nullptr };
		ast_node * node_child  { nullptr };

		ast_node * node_prev   { nullptr };
		ast_node * node_next   { nullptr };

		int line { -1 };
		int col  { -1 };
		int type_id { -1 };

		int    char_sz { -1 }; // very unecessary, we can just template this instead
		void * start { nullptr };
		void * end   { nullptr };
	};
}