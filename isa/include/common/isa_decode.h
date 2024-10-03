#include <cstdint>
#include <vector>

#ifndef INC_ISA_DECODE_H
#define INC_ISA_DECODE_H

// TODO: Add offset register support to jump/load/call instructions.
// TODO: SYSTARGET should really just have an IDT entry.

namespace isa {

// Instruction components.
struct DecodedInstruction {
    std::vector<uint32_t>* operands;
    uint8_t instruction;
};

// Decode a given instruction and return the pieces.
DecodedInstruction* decode_instr(uint32_t instr) {
    return nullptr;
}

}

#endif  // INC_ISA_DECODE_H