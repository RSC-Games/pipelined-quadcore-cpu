#include <cstdint>

#include "core/memory.h"
#include "core/bootrom.h"

#ifndef INC_CORE_MMU_H
#define INC_CORE_MMU_H

namespace core {

// Virtual Memory Address Space Layout:
// 0x00000000 - 0x00003EFF: Not memory mapped for illegal address trapping. Only accessible in kernel mode.
// 0x00003F00 - 0x00007EFF: CPU Control Registers. Only accessible in kernel mode.
// 0x00007F00 - 0x00007FFF: Interrupt Vector Table (For exceptions and interrupts). Only accessible in kernel mode.
// 0x00008000 - 0xFFFF5FFF: Program memory space (stack starts at the high end of memory, at 0xFFFF5FFF, and grows down). Accessible in Kernel or User mode.
// 0xFFFF6000 - 0xFFFFDFFF: Memory-mapped I/O. Allows direct memory access to certain registers instead of through I/O space. Only accessible in kernel mode
// 0xFFFFE000 - 0xFFFFFFFF: Memory mapped ROM code space. Max space is 128 kB. CPU hardwired to start in this address region (PC@0xFFFFFFFC). Only accessible in kernel mode; may be unmapped after boot.
// See ./soc/isa_design.txt for more information.
class MMU {
private:
    core::Memory* phy_mem;
    core::BootROM* bootrom;

public:
    MMU(core::Memory* memory, core::BootROM* bootrom);
    MMU(MMU&& other) = delete;
    MMU(const MMU& other) = delete;
    ~MMU();

    // TODO: Inline?
    uint32_t tlb_translate(uint32_t vaddr);

    uint32_t load_word(uint32_t vaddr);
    void store_word(uint32_t vaddr, uint32_t word);
    uint8_t load_byte(uint32_t vaddr);
    void store_byte(uint32_t vaddr, uint8_t word);
};
}

#endif  // INC_CORE_MMU_H