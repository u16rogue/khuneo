#include <khuneo/compiler/bc_compiler.hpp>
#include <khuneo/compiler/parser.hpp>
#include <khuneo/string_literals.hpp>

static auto bcc_resize(khuneo::compiler::bccomp_info * b) -> bool
{
	constexpr int grow_size = 18;
	int new_size = b->bc_buffer_size + grow_size;
	khuneo::kh_bytecode_t * buffer = reinterpret_cast<khuneo::kh_bytecode_t *>(b->kh_alloc(new_size));
	if (!buffer)
		return false;

	for (int i = 0; i < new_size - grow_size; ++i)	
		buffer[i] = b->bc_buffer[i];

	b->kh_free(b->bc_buffer, new_size);
	b->bc_buffer = buffer;

	return true;
}

template <int sz>
static auto tok_cmp(const khuneo::string_literal<sz> & sl, const khuneo::ast::node * const n) -> bool
{
	return sl.hash == n->tok_id;
}

template <typename T>
struct bc_generate_except_on_exit
{
	bc_generate_except_on_exit(T v_)
		: v(v_)
	{}

	~bc_generate_except_on_exit()
	{
		v();
	}

	T v;
};

namespace toks = khuneo::impl::toks;

auto khuneo::compiler::bc_compile(bccomp_info * pbcci) -> bool
{
    ast::node *  cnode      = nullptr; // current node 
	const char * except_msg = nullptr; // If except_msg has a valid value bc_compile will generate an exception

	bc_generate_except_on_exit _sg = [&]
	{
		if (except_msg && pbcci->bc_except)
		{
			bc_compiler_exception bce {};
			bce.message = except_msg;
			bce.node = cnode;
			pbcci->bc_except(&bce);
		}
	};

	if (!pbcci || !pbcci->kh_alloc || !pbcci->kh_free || !pbcci->ast_root_node)
		return false;

	if (!bcc_resize(pbcci))
		return false;

	const auto insert_byte = [&]()
	{

	};

	cnode = pbcci->ast_root_node;
	if (!tok_cmp(toks::ROOT_NODE, cnode) || !cnode->next)
	{
		except_msg = "Invalid root node!";
		return false;
	}

	while (cnode)
	{
		switch (cnode->tok_id)
		{
			case toks::EXPORT_MODULE.hash:
			{
					
				break;
			}
		};

		cnode = cnode->next;
	}

	
	return true;
}