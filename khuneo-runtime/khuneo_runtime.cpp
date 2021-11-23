#include "khuneo_runtime.hpp"
#include <cstdio>
#include <khuneo.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

auto dump_context(KHUNEO_CTX_PARAM) -> void
{
    printf("\nContext:"
           "\n\tStack pointer: 0x%p"
           "\n\tStack first: 0x%p"
           "\n\tStack last: 0x%p"
           "\n\tState: 0x%p"
           "\n\tInterrupt handler: 0x%p"
           "\n\tInterrupt flag: %c"
           "\n\tException handler: 0x%p"
           "\n\tInstruction pointer: 0x%p"
           "\n\tStack size: %llu",
           KHUNEO_CTX.registers.stack_pointer,
           KHUNEO_CTX.stack_first,
           KHUNEO_CTX.stack_last,
           KHUNEO_CTX.state,
           KHUNEO_CTX.interrupt_handler,
           KHUNEO_CTX.registers.interrupt_flag,
           KHUNEO_CTX.exception_handler,
           KHUNEO_CTX.registers.instruction_pointer,
           sizeof(KHUNEO_CTX.stack)
    );

    printf("\nRegisters: ");
    int r_count = 0;
    for (const auto & r : KHUNEO_CTX.registers.r)
        printf("\n\tr%d = [%d] %f", r_count++, r.tag, r.value.kh_number);

    printf("\nStack: ");
    for (const auto * s = KHUNEO_CTX.registers.stack_pointer; s != KHUNEO_CTX.stack_last + 1; ++s)
        printf("\n\t0x%p = [%d] %f", s, s->tag, s->value.kh_number);
}

auto vm_interrupt_handler(KHUNEO_CTX_PARAM) -> void
{
    switch (KHUNEO_CTX.registers.interrupt_flag)
    {
        case 'a':
            dump_context(KHUNEO_CTX);
            break;
        case 'm':
            printf("vm message interrupt (0x%p @ 0x%p): %s\n", &KHUNEO_CTX, KHUNEO_CTX.registers.instruction_pointer, KHUNEO_CTX.registers.r0.value.ptr);
            break;
        default:
            printf("Invalid interrupt flag\n");
            break;
    };
}

auto vm_exception_handler(KHUNEO_CTX_PARAM, khuneo::vm::exceptions ex) -> void
{
    printf("vm exception (0x%p)", &KHUNEO_CTX);

    switch (ex)
    {
        case khuneo::vm::exceptions::INVALID_OPCODE:
        {
            printf(": invalid opcode [%X %X %X %X]",
                   KHUNEO_CTX.registers.instruction_pointer[0],
                   KHUNEO_CTX.registers.instruction_pointer[1],
                   KHUNEO_CTX.registers.instruction_pointer[2],
                   KHUNEO_CTX.registers.instruction_pointer[3]
            );
            break;
        }

        case khuneo::vm::exceptions::INVALID_INTERRUPT_CODE:
            printf(": Invalid interrupt code [0x%x | %c]", KHUNEO_CTX.registers.interrupt_flag, KHUNEO_CTX.registers.interrupt_flag);
            break;

        case khuneo::vm::exceptions::NO_INTERRUPT_HANDLER:
            printf(": No interrupt handler found! IP: 0x%p", KHUNEO_CTX.registers.instruction_pointer);
            break;

        default:
            printf(": unknown exception");
            break;
    }

    printf("\n");
}

auto main(int argc, char ** argv) -> int
{
    std::string _filepath;
    const char * filepath = nullptr;
    if (argc < 2)
    {
        printf("khuneo executable (.bun) >> ");
        std::getline(std::cin, _filepath);
        filepath = _filepath.c_str();
    }
    else
    {
        filepath = argv[1];
    }

    /* if you're using VS with clang, this is not an error. */
    if (!std::filesystem::exists(filepath))
    {
        printf("Invalid .bun file path!");
        return 0;
    }

    std::ifstream bin (filepath, std::ifstream::binary);
    bin.seekg(0, bin.end);
    auto len = bin.tellg();
    bin.seekg(0, bin.beg);

    auto code = std::make_unique<char[]>(len);
    auto end  = code.get() + len;

    bin.read(code.get(), len);

    khuneo::vm::impl::context ctx {
        .interrupt_handler = vm_interrupt_handler,
        .exception_handler = vm_exception_handler
    };
    
    khuneo::vm::execute(ctx, code.get(), end);

    return 0;
}
