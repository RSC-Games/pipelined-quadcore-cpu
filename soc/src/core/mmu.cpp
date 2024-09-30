#include "core/memory.h"
#include "core/mmu.h"
#include "util/logger.h"

namespace core {

MMU::MMU() {
    LOG_WARNING("No MMU initialization performed!");
}

MMU::~MMU() {
    LOG_INFO("Destructor called; doing nothing.");
}

uint32_t MMU::tlb_translate(uint32_t vaddr) {
    // TODO: Add page table and virtual address translation.
    LOG_WARNING("READ: MMU not performing virtual address translation!");
    return vaddr;
}

}