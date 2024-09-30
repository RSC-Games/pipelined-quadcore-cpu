#include <string>
#include <iostream>
#include <csignal>
#include <cstdlib>

#include "soc.h"


void err_segv(int sig) {
    std::cout << "got signal " << sig << "\n";
    std::cout << "Segmentation Fault\n";
    std::cout << "Bad boy. Your code did illegal things. Go learn scratch and try to learn how to code.\n";
    exit(-1);
}

int main() {
    signal(SIGSEGV, err_segv);
    signal(SIGFPE, err_segv);

    // Create the emulated chip.
    core::SoCConfigurationRing config_ring = {
        1,   // Core count. 4 for testing and production.
        256, // Memory size in MB.
        64   // Clock speed in MHz.
    };

    core::SoC* soc = new core::SoC(config_ring);
    soc->simulate();
    
    return 0;
}
