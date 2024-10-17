#include <cstdlib>
#include "core/memory.h"
#include "common/interrupts.h"

#include "util/logger.h"

namespace core {

Memory::Memory(size_t size) {
    this->mem_bytes = new uint8_t[size];
    this->mem_size = size;
}

Memory::~Memory() {
    free(this->mem_bytes);
    this->mem_size = 0;
}

uint32_t Memory::load_word(uint32_t phy_addr) {
    if (phy_addr % 4 != 0) {
        // Unaligned memory access. Fail hard here.
        throw isa::INT_LOAD_STORE_ALIGNMENT_FAULT;
    }

    if (phy_addr >= mem_size - sizeof(phy_addr))
        // The requested address doesn't exist in physical memory.
        throw isa::INT_LOAD_FAULT;

    // Read an int from an arbitrary offset in one go.
    uint32_t* p_addr = reinterpret_cast<uint32_t*>(this->mem_bytes) + phy_addr;
    return *p_addr;
}

void Memory::store_word(uint32_t phy_addr, uint32_t word) {
    if (phy_addr % 4 != 0)
        // Unaligned memory access.
        throw isa::INT_LOAD_STORE_ALIGNMENT_FAULT;

    if (phy_addr >= mem_size - sizeof(phy_addr))  // Prevent writes straddling the end of the mem boundary.
        // The requested address doesn't exist in physical memory.
        throw isa::INT_STORE_FAULT;

    // Store an entire word in 1 instruction.
    uint32_t* p_addr = reinterpret_cast<uint32_t*>(this->mem_bytes) + phy_addr;
    *p_addr = word;
}

uint8_t Memory::load_byte(uint32_t phy_addr) {
    if (phy_addr >= mem_size)
        // Requested address not in physical memory.
        throw isa::INT_LOAD_FAULT;

    // No tricks this time. Just directly load the byte.
    return this->mem_bytes[phy_addr];
}

void Memory::store_byte(uint32_t phy_addr, uint8_t byte_val) {
    if (phy_addr >= mem_size)
        // Requested address not in physical memory.
        throw isa::INT_STORE_FAULT;

    this->mem_bytes[phy_addr] = byte_val;
}
}