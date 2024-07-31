#include "src/core/memory.h"

#ifndef INC_CORE_MMU_H
#define INC_CORE_MMU_H

namespace core {

class MMU {
    core::Memory* phy_memory;

public:
    MMU(core::Memory* mem);
    MMU(MMU&& other) = delete;
    MMU(const MMU& other) = delete;
    ~MMU();

    uint32_t load_word(uint32_t vaddr);
    void store_word(uint32_t vaddr, uint32_t word);
};
}

#endif  // INC_CORE_MMU_H