#include <cstdint>
#include <vector>

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