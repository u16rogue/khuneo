#include <khuneo/compiler/lang_impl.hpp>

#include <khuneo/xxhash.hpp>
#include <khuneo/runtime/vm.hpp>
#include <khuneo/compiler/parser.hpp>
#include <khuneo/string_literals.hpp>

// TODO: rewrite to generic container

static auto bcc_except(khuneo::impl::compiler::bccomp_info * b, khuneo::ast::node * n, bool fatal, const char * msg) -> void
{
	if (!b->bc_except)
		return;

	khuneo::bc_compiler_exception bcce {};
	bcce.type = fatal ? khuneo::bc_compiler_exception_type::FATAL : khuneo::bc_compiler_exception_type::WARNING;
	bcce.message = msg;
	bcce.node = n;

	b->bc_except(&bcce);
}

template <int sz>
static auto bcc_get_node_content(char (&buffer)[sz], khuneo::ast::node * n) -> int
{
	int len { 0 };
	for (const char * c = n->start; c != n->end && *c && len < sz - 1; ++len, ++c)
		buffer[len] = *c;

	buffer[len] = 0x0;

	return len;
}

// TODO: should grow based off what we insert
static auto bcc_grow(khuneo::impl::compiler::bccomp_info * b, int grow_size = 16) -> bool
{
	int index  = b->bc_buffer - b->bc_current;
	int old_size = (b->bc_end - b->bc_buffer);
	int new_size =  old_size + grow_size;

	khuneo::kh_bytecode_t * buffer = reinterpret_cast<khuneo::kh_bytecode_t *>(b->kh_alloc(new_size));
	if (!buffer)
	{
		bcc_except(b, nullptr, true, "Failed to allocate memory when growing bc_buffer");
		return false;
	}

	for (int i = 0; i < old_size; ++i)	
		buffer[i] = b->bc_buffer[i];

	b->kh_dealloc(b->bc_buffer, old_size);
	b->bc_buffer  = buffer;
	b->bc_current = &buffer[index];
	b->bc_end     = &buffer[new_size];

	return true;
}

// Automatically resizes the buffer if it requires more space
static auto bcc_auto_resize(khuneo::impl::compiler::bccomp_info * b, int num) -> bool
{
	const int res_size = b->bc_end - b->bc_buffer;
	const int index = b->bc_current - b->bc_buffer;
	if (index + 1 + num < res_size) // We should only re alloc if the current buffer is unfit
		return true;

	int best_size = num;
	// Allocate at 16 byte blocks interval so we wont have to re-allocate too much
	if (best_size % 16)
		best_size += 16 - best_size % 16;
	
	return bcc_grow(b, best_size);
}

template <typename... vargs_t>
static auto bcc_append_bytes(khuneo::impl::compiler::bccomp_info * b, vargs_t... vargs) -> bool
{
	if (!bcc_auto_resize(b, (sizeof(vargs) + ...)))
		return false;
	
	([&]
	{
		if constexpr (sizeof(vargs) == 1)
		{
			*b->bc_current = *reinterpret_cast<unsigned char *>(&vargs);
			++b->bc_current; 
		}
		else if constexpr (sizeof(vargs) == sizeof(khuneo::xxh::xxh32))
		{
			*reinterpret_cast<khuneo::xxh::xxh32 *>(b->bc_current) = vargs;
			b->bc_current += sizeof(khuneo::xxh::xxh32);
		}
		else
		{
			bcc_except(b, nullptr, true, "Could not determine type to append bytes into");	
			return false;
		}
	} (), ...);
	return true;
}

template <int sz>
static auto tok_cmp(const khuneo::string_literal<sz> & sl, const khuneo::ast::node * const n) -> bool
{
	return sl.hash == n->tok_id;
}

auto khuneo::impl::lang::rule_moduleexport::compile(impl::compiler::bccomp_info * i) -> bool
{
	if (!tok_cmp(toks::EXPORT_MODULE, i->current_node))
		return false;

	ast::node * n = i->current_node;

	if (!n->child)
	{
		bcc_except(i, n, true, "Expected a valid export node");
		return false;
	}

	n = n->child;

	// The parser already guarantees that the child is a symbol, no need to check.
	khuneo::impl::vm::op_descriptor descriptor {};
	descriptor.op_set.destination_type = khuneo::impl::vm::op_type::SYMBOL;
	descriptor.op_set.source_type      = khuneo::impl::vm::op_type::INTERMIDIATE;
	descriptor.op_set.source_size      = khuneo::impl::vm::op_size::FOUR;

	constexpr auto h_id_modulename = khuneo::xxh::hash32_cv("__modulename__");
	
	char sz_buffer[256] {};
	int len = bcc_get_node_content(sz_buffer, n);

	if (!tok_cmp(toks::SYMBOL, n) || !bcc_append_bytes(i, 
					 khuneo::impl::vm::opcodes::DEFINE, h_id_modulename,
					 khuneo::impl::vm::opcodes::COPY, descriptor, h_id_modulename, khuneo::xxh::hash32(sz_buffer, len)
	)) {
		bcc_except(i, n, true, "Failed to compile export symbol");
		return false;
	}

	n = n->next;
	if (!n || tok_cmp(toks::END_STATEMENT, n))
		return true;

	// The parser also already guarantees that the current node should be an export properties if the previous
	// check didnt fail

	khuneo::ast::node * property_group = n->child;
	while (property_group)
	{
		if (!tok_cmp(toks::COMMA_SEPARATED_GROUP, property_group))
		{
			bcc_except(i, n, true, "Export property expected an entry");
			return false;
		}
		
		khuneo::ast::node * prop_sym = n->child;
		property_group  = property_group->next;
	}

	return true;
}