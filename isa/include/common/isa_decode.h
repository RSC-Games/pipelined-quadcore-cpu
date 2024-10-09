#include <cstdint>
#include <vector>
#include <map>

#ifndef INC_ISA_DECODE_H
#define INC_ISA_DECODE_H

namespace isa {

struct DecodedInstruction {
    uint32_t* operands;
    uint8_t operand_cnt;
    uint8_t instruction;
};

DecodedInstruction* _decode_instr_only(uint32_t instr);
DecodedInstruction* _decode_instr_1reg(uint32_t instr);
DecodedInstruction* _decode_instr_2reg(uint32_t instr);
DecodedInstruction* _decode_instr_3reg(uint32_t instr);
DecodedInstruction* _decode_instr_4reg(uint32_t instr);
DecodedInstruction* _decode_instr_20bimm(uint32_t instr);
DecodedInstruction* _decode_instr_reg_imm(uint32_t instr);
DecodedInstruction* _decode_instr_reg_16bimm(uint32_t instr);

// See ./isa/src/common/isa_decode.cpp for implementation details.
DecodedInstruction* (*instr_dispatch[256])(uint32_t instr);

// Decode a given instruction and return the pieces.
DecodedInstruction* decode_instr(uint32_t instr) {
    return nullptr;
}

}

#endif  // INC_ISA_DECODE_H