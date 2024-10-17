#include "core/core.h"
#include "core/memory.h"
#include "core/mmu.h"
#include "soc.h"

#include "common/interrupts.h"
#include "common/isa_decode.h"

#include "util/logger.h"
#include "config.h"

#include <iostream>
#include <cstdlib>

// TODO: Make timing library that hides this.
#if __MINGW32__
    #include <Windows.h>
#elif __linux__
    #include <unistd.h>
#endif

namespace core {

Core::Core(core::Memory* mem, core::BootROM* bootrom) {
    this->register_file = new unsigned int[16];
    this->pc = 0xFFFFFFFC;  // Core boots 4 bytes before the end of addressable memory.
    this->priv_lvl = core::PRIV_0_KERNEL_MODE; // Core starts in kernel mode.
    this->mmu = new core::MMU(mem, bootrom);

    // Initialize pipeline.
    this->DECODE_next_instr = 0x29000000; // NOP
    this->EXECUTE_decoded_instr = new isa::DecodedInstruction {
        nullptr, // No operands.
        0,
        0x29
    };
}

Core::~Core() {
    delete this->register_file;
    this->pc = 0x0;
    this->priv_lvl = core::PRIV_1_USER_MODE;
    this->halted = true;
}

void Core::CORE_set_halt_state(bool halt) {
    this->halted = halt;
}

void Core::CORE_set_clock_rate(uint32_t clock_rate_MHz) {
    this->clock_rate = 1000000 * clock_rate_MHz;
}

void Core::simulate() {
    // TODO: Clock simulation. Currently just runs at maximum possible
    // speed.
    while (true) {
        try {
            this->CORE_run_pipeline();
        
            // TODO: make portable.
            LOG_INFO("Executed thread... waiting for next instruction.");
            // TODO: Make sleep library... maybe?
            sleep(1);
        }
        // TODO: Catch emulated CPU exceptions. Currently unsupported.
        catch (uint32_t exc) {
            this->print_exception_frame(exc);

            // TODO: Exceptions cause a jump to a handler. Do this.
        }
    }
}

void Core::print_exception_frame(uint32_t exception) {
    LOG_ERROR("Received uncaught CPU exception!");

    // TODO: Change to a lookup table?
    std::string exception_name = "UNDEFINED";

    switch (exception) {
        case isa::INT_LOAD_FAULT:
            exception_name = "LoadFault";
            break;
        case isa::INT_STORE_FAULT:
            exception_name = "StoreFault";
            break;
        case isa::INT_PAGE_FAULT:
            exception_name = "PageFault";
            break;
        case isa::INT_INVALID_PAGE_FAULT:
            exception_name = "InvalidPageFault";
            break;
        case isa::INT_LOAD_STORE_ALIGNMENT_FAULT:
            exception_name = "LoadStoreAlignmentFault";
            break;
        case isa::INT_INVALID_OPCODE_FAULT:
            exception_name = "InvalidOpcodeFault";
            break;
        case isa::INT_INSTRUCTION_FETCH_FAULT:
            exception_name = "InstructionFetchFault";
            break;
        case isa::INT_INSTR_FETCH_ALIGNMENT_FAULT:
            exception_name = "ZeroDivisionFault";
            break;
        case isa::INT_DOUBLE_FAULT:
            exception_name = "DoubleFault";
            break;
    }

    std::cout << "Fatal: " << exception_name << " encountered at PC 0x" << std::hex << this->pc << "\n";
    std::cout << "Execution trapped. Hit enter to continue. \n> ";

    std::string _ = "";
    std::getline(std::cin, _);
}

void Core::CORE_instr_fetch() {
    LOG_INFO("PIPELINE STAGE 1");
    // TODO: Store the instruction for the next pipeline stage.
    uint32_t phy_addr = this->mmu->tlb_translate(this->pc);
    uint32_t instruction = this->mmu->load_word(phy_addr);
    LOG_INFO("Fetched instruction!");

    std::cout << "Instruction opcode " << std::hex << instruction << "\n";
    this->DECODE_next_instr = instruction;
    // TODO: Store the fetched instruction for the next pipeline stage.
}

void Core::CORE_decode_instr() {
    LOG_INFO("PIPELINE STAGE 2:");

    isa::DecodedInstruction* decoded = isa::decode_instr(DECODE_next_instr);
    this->EXECUTE_decoded_instr = decoded;
}

void Core::CORE_execute_op() {
    // TODO: Build another dispatch table/huge switch and execute the instruction.
    LOG_INFO("PIPELINE STAGE 3:");
    isa::DecodedInstruction* decoded = this->EXECUTE_decoded_instr;
    std::cout << "instr " << std::hex << decoded->instruction << " op cnt " << decoded->operand_cnt << "\n";
}

void Core::CORE_write_back() {
    // TODO: Write back the cached ALU output to registers (if necessary), then
    // retire the instruction.
}

void Core::CORE_run_pipeline() {
    // Due to internal implementation details, pipeline stages will be executed in reverse
    // to avoid requiring extra memory per stage.
    this->CORE_execute_op();
    // TODO: Operand fetch stage?
    this->CORE_decode_instr();
    this->CORE_instr_fetch();

    // DEBUGGING: Print current pipeline state.
    #if VERBOSE_CPU
        LOG_WARNING("Current pipeline state:");
        std::cout << "Fetch: PC 0x" << std::hex << this->pc << "\n";
        std::cout << "Decode: Opcode 0x" << std::hex << this->DECODE_next_instr << "\n";
        std::cout << "Fetch Data? ??? \n";
        std::cout << "Execute: Decoded 0x"<< std::hex << (this->EXECUTE_decoded_instr->instruction) << " op cnt " 
            << this->EXECUTE_decoded_instr->operand_cnt << "\n";
        std::cout << "Write Back: (unimplemented)" << "\n\n";
    #endif
}

}