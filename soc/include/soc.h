#include "core/core.h"
#include "core/memory.h"

#ifndef INC_CORE_SOC
#define INC_CORE_SOC

namespace core {

// Configuration data for this SoC. Requires core_count, mem_size_MB, and clock_speed_MHz.
struct SoCConfigurationRing {
    // Available CPU cores for SoC creation. Must be between 0-4.
    uint8_t core_count;
    // Available memory for the SoC. Must be between 16 MB - 4096 MB
    size_t mem_size_MB;
    // Initial CPU clock speed. Must be between 1 MHz - 512 MHz
    uint32_t clock_speed_MHz;
};

class SoC {
    uint8_t core_count;
    core::Core** cores;
    core::Memory* memory;
    core::BootROM* bootrom;

public:
    SoC(const core::SoCConfigurationRing& config_ring);
    SoC(const SoC& other) = delete;
    SoC(SoC&& other) = delete;
    ~SoC();

    void simulate();

    inline core::Memory* get_memory();
};

}

#endif