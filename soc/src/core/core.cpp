#include "src/core/core.h"
#include "src/soc.h"

#include "common/isa_decode.h"

namespace core {

Core::Core(core::Memory* mem) {
    this->register_file = new unsigned int[16];
    this->pc = 0xFFFFFFFB;  // Core boots 4 bytes before the end of addressable memory.
    this->priv_lvl = core::PRIV_0_KERNEL_MODE; // Core starts in kernel mode.
    this->mmu = new core::MMU(mem);
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

void Core::CORE_instr_fetch() {
    // TODO: Store the instruction for the next pipeline stage.
    uint32_t instruction = this->mmu->load_word(this->pc);
    // NOTE: DOES NOTHING!!!!
}

void Core::CORE_decode_instr() {
    // TODO: Get fetched instruction from last pipeline stage.
    uint32_t instruction = 0; // Get instruction from somewhere!
    // TODO: Huge LUT required for decoding the instruction.
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