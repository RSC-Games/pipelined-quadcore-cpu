#include <cstdint>

namespace core {
class Memory {
    uint8_t* mem_bytes;
    size_t mem_size = 0;

public:
    Memory(size_t size);
    Memory(const Memory& other) = delete;
    Memory(Memory&& other) = delete;
    ~Memory();

    // Load memory from a provided aligned physical address.
    uint32_t load_word(uint32_t phy_addr);

    // Store a word at a provided aligned physical address.
    void store_word(uint32_t phy_addr, uint32_t word);

    // Load a byte at an arbitrary physical address.
    uint8_t load_byte(uint32_t phy_addr);

    // Store a byte at an arbitrary physical address.
    void store_byte(uint32_t phy_addr, uint8_t byte_val);
};
}