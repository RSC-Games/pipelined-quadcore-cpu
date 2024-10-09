#include "core/core.h"
#include "core/interrupts.h"
#include "core/memory.h"
#include "core/mmu.h"
#include "soc.h"

#include "common/isa_decode.h"

#include "util/logger.h"

#include <iostream>
#include <cstdlib>

// TODO: make portable
#if __MINGW32__
    #include <Windows.h>
#elif __linux__
    #include <unistd.h>
#endif

namespace core {

Core::Core(core::Memory* mem) {
    this->register_file = new unsigned int[16];
    this->pc = 0xFFFFFFFC;  // Core boots 4 bytes before the end of addressable memory.
    this->priv_lvl = core::PRIV_0_KERNEL_MODE; // Core starts in kernel mode.
    this->memory = mem;
    this->mmu = new core::MMU();
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
            //sleep(1);
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
        case core::INT_LOAD_FAULT:
            exception_name = "LoadFault";
            break;
        case core::INT_STORE_FAULT:
            exception_name = "StoreFault";
            break;
        case core::INT_PAGE_FAULT:
            exception_name = "PageFault";
            break;
        case core::INT_INVALID_PAGE_FAULT:
            exception_name = "InvalidPageFault";
            break;
        case core::INT_LOAD_STORE_ALIGNMENT_FAULT:
            exception_name = "LoadStoreAlignmentFault";
            break;
        case core::INT_INVALID_OPCODE_FAULT:
            exception_name = "InvalidOpcodeFault";
            break;
        case core::INT_INSTRUCTION_FETCH_FAULT:
            exception_name = "InstructionFetchFault";
            break;
        case core::INT_INSTR_FETCH_ALIGNMENT_FAULT:
            exception_name = "ZeroDivisionFault";
            break;
        case core::INT_DOUBLE_FAULT:
            exception_name = "DoubleFault";
            break;
    }

    std::cout << "Fatal: " << exception_name << " encountered at PC 0x" << std::hex << this->pc << "\n";
    std::cout << "Execution trapped. Hit enter to continue. \n> ";

    std::string _ = "";
    std::getline(std::cin, _);
}

void Core::CORE_instr_fetch() {
    // TODO: Store the instruction for the next pipeline stage.
    uint32_t phy_addr = this->mmu->tlb_translate(this->pc);
    uint32_t instruction = this->memory->load_word(phy_addr);
    LOG_INFO("Fetched instruction!");
    std::cout << "Instruction opcode " << std::hex << instruction << "\n";
    //this->
    // TODO: Store the fetched instruction for the next pipeline stage.
}

void Core::CORE_decode_instr() {
    // TODO: Get fetched instruction from last pipeline stage.
    uint32_t instruction = 0;
    // TODO: Use isa_decode.h to decode the instruction.
}

void Core::CORE_execute_op() {
    // TODO: Build dispatch table and execute the instruction.
}

void Core::CORE_write_back() {
    // TODO: Write back the cached ALU output to registers (if necessary), then
    // retire the instruction.
}

void Core::CORE_run_pipeline() {
    // Due to internal implementation details, pipeline stages will be executed in reverse
    // to avoid requiring extra memory per stage.
    this->CORE_instr_fetch();
}

}