cmake_minimum_required(VERSION 3.18)

add_library(khuneo-lang INTERFACE
	"khuneo.hpp"
	"khuneo_vm.hpp"
	"khuneo_state.hpp"
	
	"khuneo_vm_ops.hpp"
	"khuneo_vm_context.hpp"
	"khuneo_data.hpp"
)

set_target_properties(khuneo-lang PROPERTIES LINKER_LANGUAGE CXX)
set(khuneo-lang-includes "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)