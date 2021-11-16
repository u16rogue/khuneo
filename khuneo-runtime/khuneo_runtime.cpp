#include <cstdio>
#include <khuneo.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include "khuneo_runtime.hpp"

auto vm_interrupt_handler(KHUNEO_CTX_PARAM) -> void
{
    if (KHUNEO_CTX.registers.interrupt_flag == 'm')
    {
        printf("\nvm message interrupt (0x%p @ 0x%p): %s", &KHUNEO_CTX, KHUNEO_CTX.registers.ip.ptr, KHUNEO_CTX.registers.r0.ptr);
    }
}

auto vm_exception_handler(KHUNEO_CTX_PARAM, khuneo::vm::exceptions ex) -> void
{
    printf("\nvm exception (0x%p)", &KHUNEO_CTX);

    switch (ex)
    {
        case khuneo::vm::exceptions::INVALID_OPCODE:
        {
            printf(": invalid opcode [%X %X %X %X]",
                   KHUNEO_CTX.registers.ip.ptr[0],
                   KHUNEO_CTX.registers.ip.ptr[1],
                   KHUNEO_CTX.registers.ip.ptr[2],
                   KHUNEO_CTX.registers.ip.ptr[3]
            );
            break;
        }

        case khuneo::vm::exceptions::INVALID_INTERRUPT_CODE:
            printf(": Invalid interrupt code [0x%x | %c]", KHUNEO_CTX.registers.interrupt_flag, KHUNEO_CTX.registers.interrupt_flag);
            break;

        case khuneo::vm::exceptions::NO_INTERRUPT_HANDLER:
            printf(": No interrupt handler found! IP: 0x%p", KHUNEO_CTX.registers.ip.ptr);
            break;

        default:
            printf(": unknown exception");
            break;
    }
}

auto main(int argc, char ** argv) -> int
{
    if (argc < 2)
    {
        printf("Insufficient parameters.");
        return 1;
    }

    if (!std::filesystem::exists(argv[1]))
    {
        printf("Input file not found.");
        return 0;
    }

    std::ifstream bin (argv[1], std::ifstream::binary);
    bin.seekg(0, bin.end);
    auto len = bin.tellg();
    bin.seekg(0, bin.beg);

    auto code = std::make_unique<char[]>(len);
    auto end  = code.get() + len;

    bin.read(code.get(), len);

    khuneo::vm::impl::context ctx { .interrupt_handler = vm_interrupt_handler, .exception_handler = vm_exception_handler };
    khuneo::vm::execute(ctx, code.get(), end);

    return 0;
}
