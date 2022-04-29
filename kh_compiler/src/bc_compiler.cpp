#include <khuneo/compiler/bc_compiler.hpp>

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

auto khuneo::compiler::bc_compiler(bccomp_info * pbcci) -> bool
{
	if (!pbcci || !pbcci->kh_alloc || !pbcci->kh_free || !pbcci->ast_root_node)
		return false;

	if (!bcc_resize(pbcci))
		return false;



	return true;
}