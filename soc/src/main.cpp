#include <string>
#include <iostream>
#include <csignal>
#include <cstdlib>

#include "soc.h"


void err_segv(int sig) {
    std::cout << "got signal " << sig << "\n";
    std::cout << "Segmentation Fault/FPU Exception\n";
    exit(-1);
}

int main() {
    signal(SIGSEGV, err_segv);
    signal(SIGFPE, err_segv);

    // Create the emulated chip.
    core::SoCConfigurationRing config_ring = {
        1,   // Core count. 4 for testing and production.
        16, // Memory size in MB.
        64   // Clock speed in MHz.
    };

    core::SoC* soc = new core::SoC(config_ring);
    soc->simulate();
    
    return 0;
}
