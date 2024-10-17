#include <iostream>

#include "config.h"
#include "core/memory.h"
#include "core/mmu.h"
#include "util/logger.h"

namespace core {
constexpr uint32_t BOOTROM_BASE_ADDR = 0xFFFFFFFF - (sizeof(BOOTROM_BYTES)) + 1;

// TODO: Shadow BootROM to memory and initialize the MMU with the shadowed ROM mapped at 0xFFFFXXXX
MMU::MMU(core::Memory* mem, core::BootROM* bootrom) {
    LOG_WARNING("mmapping bootrom!");
    this->phy_mem = mem;
    this->bootrom = bootrom;

    std::cout << "bootrom base address " << std::hex << BOOTROM_BASE_ADDR << "\n";
}

MMU::~MMU() {
    LOG_INFO("Destructor called; doing nothing.");
}

uint32_t MMU::tlb_translate(uint32_t vaddr) {
    // TODO: Add page table and virtual address translation.
    LOG_WARNING("MMU not performing virtual address translation!");
    return vaddr;
}

uint32_t MMU::load_word(uint32_t phy_addr) {
    if (phy_addr >= BOOTROM_BASE_ADDR) {
        #if VERBOSE_MMU
            LOG_WARNING("MMU: Got bootrom address!");
            std::cout << "Looking up memory at addr " << std::hex << phy_addr << ", bootrom abs " 
                << std::hex << (phy_addr - BOOTROM_BASE_ADDR) << "\n";
        #endif

        return this->bootrom->load_word(phy_addr - BOOTROM_BASE_ADDR);
    }

    return this->phy_mem->load_word(phy_addr);
}

void MMU::store_word(uint32_t phy_addr, uint32_t word) {
    if (phy_addr >= BOOTROM_BASE_ADDR) {
        LOG_WARNING("Got bootrom address!");
        this->bootrom->store_word(phy_addr - BOOTROM_BASE_ADDR, word);
    }
    else
        this->phy_mem->store_word(phy_addr, word);
}

uint8_t MMU::load_byte(uint32_t phy_addr) {
    if (phy_addr >= BOOTROM_BASE_ADDR) {
        LOG_WARNING("Got bootrom address!");
        return this->bootrom->load_byte(phy_addr - BOOTROM_BASE_ADDR);
    }

    return this->phy_mem->load_byte(phy_addr);
}

void MMU::store_byte(uint32_t phy_addr, uint8_t word) {
    if (phy_addr >= BOOTROM_BASE_ADDR) {
        LOG_WARNING("Got bootrom address!");
        this->bootrom->store_byte(phy_addr - BOOTROM_BASE_ADDR, word);
    }
    else
        this->phy_mem->store_byte(phy_addr, word);
}

}