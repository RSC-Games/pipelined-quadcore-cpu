#include <cstdint>
#include <iostream>
#include <vector>
#include <map>

#include <common/interrupts.h>

#include <util/logger.h>

#ifndef INC_ISA_DECODE_H
#define INC_ISA_DECODE_H

// Conditional types:
#define CMP_COND_EQUIVALENT         0
#define CMP_COND_LESS_THAN          1
#define CMP_COND_GREATHER_THAN      2
#define CMP_COND_LESS_OR_EQUAL      3
#define CMP_COND_GREATER_OR_EQUAL   4
#define CMP_COND_NOT_EQUAL          5

#define BNZ_COND_NOT_ZERO           0
#define BNZ_COND_ZERO               1

namespace isa {

struct DecodedInstruction {
    uint32_t* operands;
    uint8_t operand_cnt;
    uint16_t instruction;
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
extern DecodedInstruction* (*instr_dispatch[256])(uint32_t);

// Pipeline bubbling (exception/failed decode/etc)
DecodedInstruction* get_empty_instr();

// Decode a given instruction and return the pieces.
DecodedInstruction* decode_instr(uint32_t instr);

}

#endif  // INC_ISA_DECODE_H