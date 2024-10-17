#include <cstdint>
#include <stddef.h>

#ifndef __core_bootrom_h
#define __core_bootrom_h

namespace core {
const uint8_t BOOTROM_BYTES[4] = {
    0x00, 0x12, 0x10, 0x34  // bnx NZ X0 X1 X2
};

class BootROM {
    const uint8_t* rom_bytes;
    size_t rom_size = 0;

public:
    BootROM(const uint8_t* bytes, size_t size);
    BootROM(const BootROM& other) = delete;
    BootROM(BootROM&& other) = delete;
    ~BootROM();

    // Load memory from a provided aligned physical address.
    // TODO: Inline?
    uint32_t load_word(uint32_t phy_addr);

    // Store a word at a provided aligned physical address.
    void store_word(uint32_t phy_addr, uint32_t word);

    // Load a byte at an arbitrary physical address.
    uint8_t load_byte(uint32_t phy_addr);

    // Store a byte at an arbitrary physical address.
    void store_byte(uint32_t phy_addr, uint8_t byte_val);
};

}

#endif  // __core_bootrom_h