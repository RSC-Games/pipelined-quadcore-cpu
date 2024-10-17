#include <iostream>
#include "soc.h"
#include "util/logger.h"

namespace core {

core::SoC::SoC(const core::SoCConfigurationRing &config_ring) {
    if (config_ring.core_count <= 0 || config_ring.core_count > 4) {
        // Unsupported core configuration.
        LOG_ERROR("Unsupported core configuration! Got " + std::to_string(config_ring.core_count) + " cores!");
        std::abort();
    }

    if (config_ring.mem_size_MB < 16 || config_ring.mem_size_MB > 4096) {
        LOG_ERROR("Unsupported memory configuration! Got size " + std::to_string(config_ring.mem_size_MB) + " MB!");
        std::abort();
    }

    if (config_ring.clock_speed_MHz < 1 || config_ring.clock_speed_MHz > 512) {
        LOG_ERROR("Unsupported clock speed requested! Got speed " + std::to_string(config_ring.clock_speed_MHz) + " MHz!");
        std::abort();
    }

    this->core_count = config_ring.core_count;
    this->cores = new core::Core*[config_ring.core_count];

    this->memory = new Memory(config_ring.mem_size_MB * 1048576);
    this->bootrom = new BootROM(BOOTROM_BYTES, sizeof(BOOTROM_BYTES));
    
    // TODO: Put each core on its own execution thread.
    for (int i = 0; i < config_ring.core_count; i++) {
        this->cores[i] = new core::Core(this->memory, this->bootrom);
        this->cores[i]->CORE_set_clock_rate(config_ring.clock_speed_MHz);
    }
}

core::SoC::~SoC() {
    for (int i = 0; i < this->core_count; i++) {
        this->cores[i]->CORE_set_halt_state(true);
        delete this->cores[i];
    }

    delete this->cores;
    delete this->memory;
}

void core::SoC::simulate() {
    // TODO: Spawn other threads to simulate the other cores.
    this->cores[0]->simulate();

    // TODO: Figure out what else this needs to do?
}

inline core::Memory* core::SoC::get_memory() {
    return this->memory;
}

}