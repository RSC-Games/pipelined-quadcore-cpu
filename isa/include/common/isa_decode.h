#include <cstdint>
#include <vector>
#include <map>

#ifndef INC_ISA_DECODE_H
#define INC_ISA_DECODE_H

namespace isa {

struct DecodedInstruction {
    std::vector<uint32_t>* operands;
    uint8_t instruction;
};

const std::map<int, DecodedInstruction* (uint32_t)>* decoder_lut;

void fill_decoder() {
    // Instruction components.
    decoder_lut = {
        {0b00000001, _decode_instr_2reg},          // ADD SRC_REG1/DEST_REG SRC_REG2
        {0b00000010, _decode_instr_2reg},          // SUB SRC_REG1/DEST_REG SRC_REG2
        {0b00000011, _decode_instr_2reg},          // MUL SRC_REG1/DEST_REG SRC_REG2
        {0b00000100, _decode_instr_2reg},          // DIV SRC_REG1/DEST_REG SRC_REG2
                                            // NOT IMPLEMENTED: ADD_FP
                                            // NOT IMPLEMENTED: SUB_FP
                                            // NOT IMPLEMENTED: MUL_FP
                                            // NOT IMPLEMENTED: DIV_FP
        {0b00010000, _decode_instr_2reg},          // AND SRC_REG1/DEST_REG SRC_REG2
        "or": _decode_instr_2reg,           // OR SRC_REG1/DEST_REG SRC_REG2 
        "xor": _decode_instr_2reg,          // XOR SRC_REG1/DEST_REG SRC_REG2
        "not": _decode_instr_1reg,          // NOT SRC_REG/DEST_REG

        "mov": _decode_instr_2reg,          // MOV DEST_REG SRC_REG
        "mov_imm": _decode_instr_reg_imm,   // MOV DEST_REG IMM_20_BIT
        "mov_imm_high": _decode_instr_reg_16bimm, // MOVH DEST_REG IMM_16_BIT (MOV HIGH 16 BITS)
        "mov_ext": _decode_instr_2reg,      // MOV DEST_EXT_REG SRC_REG
        "mov_gp": _decode_instr_2reg,       // MOV DEST_REG SRC_EXT_REG
        "mov_ext2": _decode_instr_2reg,     // MOV DEST_EXT_REG SRC_EXT_REG
        "mov_ext_imm": _decode_instr_reg_imm, // MOV DEST_EXT_REG IMM_20_BIT

        "ldb": _decode_instr_3reg,          // LDB DEST_REG BASE_ADDR_REG OFFSET_REG
        "stb": _decode_instr_3reg,          // STB SRC_REG BASE_ADDR_REG OFFSET_REG
        "ld": _decode_instr_3reg,           // LD DEST_REG BASE_ADDR_REG OFFSET_REG
        "st": _decode_instr_3reg,           // ST SRC_REG BASE_ADDR_REG OFFSET_REG

        "lsh": _decode_instr_2reg,          // LSH OPERAND_REG SHIFT_REG
        "lsh_imm": _decode_instr_reg_imm,   // LSH OPERAND_REG IMM_20_BIT_SHIFT
        "rsh": _decode_instr_2reg,          // RSH OPERAND_REG SHIFT_REG
        "rsh_imm": _decode_instr_reg_imm,   // RSH OPERAND_REG IMM_20_BIT_SHIFT

        "jmp_imm": _decode_instr_reg_imm,   // JMP ADDR_REG IMM_20_BIT_SHIFT
        "jmp_rel": _decode_instr_2reg,      // JMP BASE_ADDR_REG OFFSET_REG
        "jmp": _decode_instr_1reg,          // JMP ADDR_REG
        "jmpc": _decode_instr_5reg,         // JMP COND OPERAND_REG1 OPERAND_REG2 BASE_ADDR_REG OFFSET_REG
        "jmpc_z": _decode_instr_4reg,       // JMPZ COND OPERAND_REG1 BASE_ADDR_REG OFFSET_REG

        "call_imm": _decode_instr_reg_imm,  // CALL ADDR_REG IMM_20_BIT_OFFSET
        "call": _decode_instr_2reg,         // CALL BASE_ADDR_REG OFFSET_REG
        "ret": _decode_instr_only,          // RET
        "push": _decode_instr_1reg,         // PUSH REG
        "pop": _decode_instr_1reg,          // POP REG
        "push_ext": _decode_instr_1reg,     // PUSH EXT_REG
        "pop_ext": _decode_instr_1reg,      // POP EXT_REG

        "syscall": _decode_instr_1reg,      // SYSCALL ID_REG
        "sysret": _decode_instr_only,       // SYSRET
        "hlt": _decode_instr_only,          // HLT
        "cpuid": _decode_instr_1reg         // CPUID DEST_REG
    };
}

DecodedInstruction* _decode_instr_only(uint32_t instr);
DecodedInstruction* _decode_instr_1reg(uint32_t instr);
DecodedInstruction* _decode_instr_2reg(uint32_t instr);
DecodedInstruction* _decode_instr_3reg(uint32_t instr);
DecodedInstruction* _decode_instr_4reg(uint32_t instr);
DecodedInstruction* _decode_instr_5reg(uint32_t instr);

// Decode a given instruction and return the pieces.
DecodedInstruction* decode_instr(uint32_t instr) {
    return nullptr;
}

}

#endif  // INC_ISA_DECODE_H