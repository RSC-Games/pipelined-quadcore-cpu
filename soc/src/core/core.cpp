#include "src/core/core.h"

namespace core {

Core::Core() {
    this->register_file = new unsigned int[16];
    this->pc = 0xFFFFFFFB;  // Core boots 4 bytes before the end of addressable memory.
    this->priv_lvl = core::PRIV_0_KERNEL_MODE; // Core starts in kernel mode.
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

}