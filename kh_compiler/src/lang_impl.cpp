#include <khuneo/compiler/lang_impl.hpp>

#include <khuneo/xxhash.hpp>
#include <khuneo/runtime/vm.hpp>
#include <khuneo/compiler/parser.hpp>
#include <khuneo/string_literals.hpp>

// TODO: rewrite to generic container

static const char * const BCC_MSG_APPEND_FAIL = "Failed to append bytes to executable buffer";

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
	int index    = b->bc_current - b->bc_buffer;
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
	// TODO: resize check should take in account of string from the beginning
	// POSSIBLE BUG: pass long string -> process non string -> process string -> re alloc based off string, chances that string might match &/mod the prev alloc -> process the rest of the non string -> no more memory -> oob
	if (!bcc_auto_resize(b, (sizeof(vargs) + ...)))
		return false;
	
	([&]
	{
		// TODO: we should seriously compare types here...
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
		else if constexpr (requires { /*const char * v = vargs;*/ ((void(*)(const char *))0)(vargs); })
		{
			int len = 0;
			while (vargs[++len]);
			bcc_auto_resize(b, len + 1);
			// Do not include the null terminator, its up to the caller if they want to append it themselves
			for (int i = 0; i < len; ++i)
				b->bc_current[i] = vargs[i];
			b->bc_current += len;
		}
		else
		{
			// TODO: this can be determined at compile time... which we should.
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
	khuneo::impl::vm::op_descriptor copy_descriptor {};
	copy_descriptor.op_copy.destination_type = khuneo::impl::vm::op_type::SYMBOL;
	copy_descriptor.op_copy.source_type      = khuneo::impl::vm::op_type::INTERMIDIATE;
	copy_descriptor.op_copy.source_size      = khuneo::impl::vm::op_size::FOUR;

	khuneo::impl::vm::op_descriptor define_descriptor {};
	define_descriptor.op_define.mode = khuneo::impl::vm::op_define_mode::HASH;
	define_descriptor.op_define.type = khuneo::impl::vm::op_define_type::SYMBOL;

	constexpr auto h_id_modulename = khuneo::xxh::hash32_cv("__modulename__");
	
	char sz_buffer[256] {};
	int len = bcc_get_node_content(sz_buffer, n);

	if (!tok_cmp(toks::SYMBOL, n) || !bcc_append_bytes(i, 
					 khuneo::impl::vm::opcodes::DEFINE, h_id_modulename,
					 khuneo::impl::vm::opcodes::COPY, copy_descriptor, h_id_modulename, khuneo::xxh::hash32(sz_buffer, len)
	)) {
		bcc_except(i, n, true, BCC_MSG_APPEND_FAIL);
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

auto khuneo::impl::lang::rule_variable::compile(impl::compiler::bccomp_info * i) -> bool
{
	if (!tok_cmp(toks::VARIABLE, i->current_node))
		return false;

	ast::node * n = i->current_node->child;

	/*
	khuneo::impl::vm::op_descriptor jmp_next_descriptor {};
	jmp_next_descriptor.op_jmp.jump_type = khuneo::impl::vm::op_jmp_type::RELATIVE;
	jmp_next_descriptor.op_jmp.displacement_source = khuneo::impl::vm::op_type::INTERMIDIATE;
	jmp_next_descriptor.op_jmp.displacement_size = khuneo::impl::vm::op_size::ONE;
	*/

	khuneo::impl::vm::op_descriptor define_descriptor {};
	define_descriptor.op_define.mode = khuneo::impl::vm::op_define_mode::STRING;
	define_descriptor.op_define.type = khuneo::impl::vm::op_define_type::SYMBOL;

	char sz_buffer[257];
	int len = bcc_get_node_content(sz_buffer, n);
	// NOTE: this limitation will be kept despite no longer requiring a JMP_NEXT instruction just incase i change my mind or something else comes up, who would need >127 length variable names anyway?
	if (len > 0x7F) // This magic number is the maximum value for a signed single byte data type since all bits are set except the most significant bit which denotes negative
	{
		bcc_except(i, n, true, "Variable name too long, max limit is 127 characters.");
		return false;
	}

	if (!bcc_append_bytes(i,
		// khuneo::impl::vm::opcodes::JMP_NEXT, jmp_next_descriptor, static_cast<char>(len + 3), // JMP_NEXT REL <SYMBOL LENGTH>
		khuneo::impl::vm::opcodes::DEFINE, define_descriptor, sz_buffer, static_cast<char>(0) /*<- Null terminator*/
	)) {
		bcc_except(i, n, true, BCC_MSG_APPEND_FAIL);
		return false;
	}

	return true;
}