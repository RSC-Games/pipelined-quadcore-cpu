#include <iostream>
#include "src/soc.h"
#include "src/util/logger.h"

namespace core {

core::SoC::SoC(const core::SoCConfigurationRing &config_ring) {
    if (config_ring.core_count <= 0 || config_ring.core_count > 4) {
        // Unsupported core configuration.
        log_error("Unsupported core configuration! Got " + config_ring.core_count);
        std::abort();
    }

    if (config_ring.mem_size_MB < 16 || config_ring.mem_size_MB > 4096) {
        log_error("Unsupported memory configuration! Got size " + config_ring.mem_size_MB);
        std::abort();
    }

    if (config_ring.clock_speed_MHz < 1 || config_ring.clock_speed_MHz > 512) {
        log_error("Unsupported clock speed requested! Got speed " + config_ring.clock_speed_MHz);
        std::abort();
    }

    this->core_count = config_ring.core_count;
    this->cores = new core::Core*[config_ring.core_count];
    
    // TODO: Put each core on its own execution thread.
    for (int i = 0; i < config_ring.core_count; i++) {
        this->cores[i] = new core::Core();
        this->cores[i]->CORE_set_clock_rate(config_ring.clock_speed_MHz);
    }

    this->memory = new Memory(config_ring.mem_size_MB);
}

core::SoC::~SoC() {
    for (int i = 0; i < this->core_count; i++) {
        this->cores[i]->CORE_set_halt_state(true);
        delete this->cores[i];
    }

    delete this->cores;
    delete this->memory;
}

}