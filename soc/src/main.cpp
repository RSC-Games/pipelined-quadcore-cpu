#include <string>
#include <iostream>
#include <csignal>
#include <cstdlib>

#include "src/soc.h"


void err_segv(int a) {
    std::cout << "Segmentation Fault\n";
    std::cout << "Bad boy. Your code did illegal things. Go learn scratch and try to learn how to code.\n";
    exit(-1);
}

int main() {
    signal(SIGSEGV, err_segv);
    signal(SIGFPE, err_segv);

    // Create the emulated chip.
    core::SoCConfigurationRing config_ring = {
        4,   // Core count. 4 for testing and production.
        256, // Memory size in MB.
        64   // Clock speed in MHz.
    };

    core::SoC soc = core::SoC(config_ring);

    // Let's do some fun crash stuff.
    //int* thing = nullptr;
    //int a_value = *thing;

    //std::cout << "Why did the code manage to get this far?\n";
    return 0;
}
