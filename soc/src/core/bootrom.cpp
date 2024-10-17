#include <iostream>

#include "core/bootrom.h"
#include "common/interrupts.h"

#include "config.h"

namespace core {

BootROM::BootROM(const uint8_t* bytes, size_t size) {
    this->rom_bytes = bytes;
    this->rom_size = size;
}

// Nothing to do here.
BootROM::~BootROM() {}

// Load memory from a provided aligned physical address.
// TODO: Inline?
uint32_t BootROM::load_word(uint32_t phy_addr) {
    if (phy_addr % 4 != 0) {
        // Unaligned memory access. Fail hard here.
        #if VERBOSE_MEMORY
            std::cout << "Unaligned access at addr " << std::hex << phy_addr << ", % 4 = " 
                << phy_addr % 4 << "\n";
        #endif

        throw isa::INT_LOAD_STORE_ALIGNMENT_FAULT;
    }

    if (phy_addr >= this->rom_size - sizeof(phy_addr) - 1) {
        #if VERBOSE_MEMORY
            std::cout << "OOB access at " << std::hex << phy_addr << ", cond=addr < " << std::hex 
                << (phy_addr >= this->rom_size - sizeof(phy_addr)) << "\n";
        #endif

        // The requested address doesn't exist in the bootrom.
        throw isa::INT_LOAD_FAULT;
    }

    // Read an int from an arbitrary offset in one go.
    const uint32_t* p_addr = reinterpret_cast<const uint32_t*>(this->rom_bytes) + phy_addr;
    return *p_addr;
}

void BootROM::store_word(uint32_t phy_addr, uint32_t word) {
    throw isa::INT_STORE_FAULT; // BootROM is not writable
}

// Load a byte at an arbitrary physical address.
uint8_t BootROM::load_byte(uint32_t phy_addr) {
    if (phy_addr >= this->rom_size)
        // Requested address not in physical memory.
        throw isa::INT_LOAD_FAULT;

    return this->rom_bytes[phy_addr];
}


void BootROM::store_byte(uint32_t phy_addr, uint8_t byte_val) {
    throw isa::INT_STORE_FAULT;
}

}