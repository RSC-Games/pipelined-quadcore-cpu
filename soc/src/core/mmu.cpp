#include "src/core/memory.h"
#include "src/core/mmu.h"
#include "src/util/logger.h"

namespace core {

MMU::MMU(core::Memory* mem) {
    this->phy_memory = mem;
}

MMU::~MMU() {
    this->phy_memory = nullptr;
}

uint32_t MMU::load_word(uint32_t vaddr) {
    // TODO: Add page table and virtual address translation.
    LOG_WARNING("MMU not performing virtual address translation on load_word!");
    return this->phy_memory->load_word(vaddr);
}

}