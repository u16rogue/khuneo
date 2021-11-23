cmake_minimum_required(VERSION 3.18)

add_library(khuneo-lang INTERFACE
	"khuneo.hpp"
	"khuneo_state.hpp"
	"khuneo_data.hpp"
	"khuneo_parameters.hpp"
	"khuneo_hash.hpp"

	"vm/khuneo_vm.hpp"
	"vm/khuneo_vm_ops.hpp"
	"vm/khuneo_vm_context.hpp"

	"vm/opcodes/khuneo_opcodes.hpp"
	"vm/opcodes/khuneo_op_nop.hpp"
	"vm/opcodes/khuneo_op_inti.hpp"
)

set_target_properties(khuneo-lang PROPERTIES LINKER_LANGUAGE CXX)
set(khuneo-lang-includes "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)