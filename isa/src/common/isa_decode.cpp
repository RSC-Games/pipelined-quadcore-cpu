#include "common/isa_decode.h"

namespace isa {

// Dispatch table contains the decoder entries at the indices directly corresponding to the
// instruction opcode. Minor speedo optimization.
DecodedInstruction* (*instr_dispatch[256])(uint32_t instr) = {
    nullptr,                // 0b00000000 - empty (illegal instruction when jumping to NOP sled to reduce attack surfaces or break infinite loops in empty memory. :D)
    _decode_instr_2reg,     // 0b00000001 - ADD <SRC register 1/DEST register> <SRC register 2> (Adds and accumulates to 1 register)
    _decode_instr_2reg,     // 0b00000010 - SUB <SRC register 1/DEST register> <SRC register 2> (Subtracts and accumulates to 1 register)
    _decode_instr_2reg,     // 0b00000011 - MUL <SRC register 1/DEST register> <SRC register 2> (Multiply and accumulates to 1 register)
    _decode_instr_2reg,     // 0b00000100 - DIV <SRC register 1/DEST register> <SRC register 2> (Adds and accumulates to 1 register)
    nullptr,                // 0b00000101 - empty (ADD_FP (no accumulate instruction here))
    nullptr,                // 0b00000110 - empty (SUB_FP (no accumulate instruction here))
    nullptr,                // 0b00000111 - empty (MUL_FP (no accumulate instruction here))
    nullptr,                // 0b00001000 - empty (DIV_FP (no accumulate instruction here))
    nullptr,                // 0b00001001 - empty
    nullptr,                // 0b00001010 - empty
    nullptr,                // 0b00001011 - empty
    nullptr,                // 0b00001100 - empty
    nullptr,                // 0b00001101 - empty
    nullptr,                // 0b00001110 - empty
    nullptr,                // 0b00001111 - empty
    _decode_instr_2reg,     // 0b00010000 - AND <SRC register 1/DEST register> <SRC register 2> (Perform bitwise AND on two registers and store the result in dest)
    _decode_instr_2reg,     // 0b00010001 - OR <SRC register 1/DEST register> <SRC register 2> (Perform bitwise OR on two registers and store the result in dest)
    _decode_instr_2reg,     // 0b00010010 - XOR <SRC register 1/DEST register> <SRC register 2>(Perform bitwise XOR on two registers and store the result in dest)
    _decode_instr_1reg,     // 0b00010011 - NOT <SRC register/DEST register> (Perform bitwise NOT on the source register)
    _decode_instr_2reg,     // 0b00010100 - MOV <DEST register> <SRC register> (Moves one register value to another without clearing the source)
    _decode_instr_reg_imm,  // 0b00010101 - MOV_IMM <DEST register> <20 bit immediate> (Move a specified immediate value into a provided register)
    _decode_instr_reg_16bimm,// 0b00010110 - MOV_IMM_HIGH <DEST register> <16 bit immediate> (Move a specified immediate value into the 2 high bytes of a register)
    _decode_instr_2reg,     // 0b00010111 - MOV_EXT <DEST EXT register> <SRC GP register>  (Move one general-purpose register value to an extended register).
    _decode_instr_2reg,     // 0b00011000 - MOV_GP <DEST GP register> <SRC EXT register> (Move one extended register value to a general-purpose register).
    _decode_instr_2reg,     // 0b00011001 - MOV_EXT2 <DEST EXT register> <SRC EXT register> (Move one extended register value to another extended register).
    _decode_instr_reg_imm,  // 0b00011010 - MOV_EXT_IMM <DEST EXT register> <20 bit immediate> (Move an immeidate into an extended register).
    nullptr,                // 0b00011011 - empty
    nullptr,                // 0b00011100 - empty
    nullptr,                // 0b00011101 - empty
    nullptr,                // 0b00011110 - empty
    nullptr,                // 0b00011111 - empty
    _decode_instr_3reg,     // 0b00100000 - LDB <DEST register> <BASE_ADDR register> <OFFSET register> (Load a byte from memory at the address register value, left shift it the given amount, then store it into the given register, overwriting all bits present at that offset. Must be byte aligned)
    _decode_instr_3reg,     // 0b00100001 - STB <SRC register> <BASE_ADDR register> <OFFSET register> (Store a byte to memory at the address register value. Right shift the byte, keep only the lowest byte, and write it to memory.)
    _decode_instr_3reg,     // 0b00100010 - LD <DEST register> <BASE_ADDR register> <OFFSET register> (Load a machine word)
    _decode_instr_3reg,     // 0b00100011 - ST <SRC register> <BASE_ADDR register> <OFFSET register> (Store a machine word)
    nullptr,                // 0b00100100 - empty
    nullptr,                // 0b00100101 - empty
    nullptr,                // 0b00100110 - empty
    nullptr,                // 0b00100111 - empty
    _decode_instr_2reg,     // 0b00101000 - LSH <OPERAND register> <SHIFT register> (Left shifts the source register by a value in the shift register)
    _decode_instr_2reg,     // 0b00101001 - LSH_IMM <OPERAND register> <20 bit immediate> (Left shifts the provided register by an immediate)
    _decode_instr_2reg,     // 0b00101010 - RSH <OPERNAD register> <SHIFT register> (Right shifts the provided register by a value in another register)
    _decode_instr_2reg,     // 0b00101011 - RSH_IMM <OPERAND register> <20 bit immediate> (Right shifts the provided register by an immediate)
    nullptr,                // 0b00101100 - empty
    nullptr,                // 0b00101101 - empty
    nullptr,                // 0b00101110 - empty
    nullptr,                // 0b00101111 - empty
    _decode_instr_reg_imm,  // 0b00110000 - JMP_IMM <BASE_ADDR register> <20 bit immediate> (For small code sizes allows extremely fast jumps within a megabyte above the provided address register)
    _decode_instr_2reg,     // 0b00110001 - JMP <BASE_ADDR register> <OFFSET register> (Jump to an offset relative to the provided address register)
    _decode_instr_1reg,     // 0b00110010 - JMP_ABS <ADDRESS register> (Jump to an absolute address in the memory space from the provided register)
    _decode_instr_20bimm,   // 0b00110011 - JMP_REL <20 bit signed immediate> (Jump unconditionally relative to the current PC)
    _decode_instr_4reg,     // 0b00110100 - BNX <1 bit conditional code> <OPERAND register> <BASE_ADDR register> <OFFSET register> (Branch. Codes include NZ (NOT ZERO), ZR (ZERO). Compare the operand with 0, and if the condition is met, jump to the absolute address in the provided register)
    _decode_instr_4reg,     // 0b00110101 - CMP <3 bit conditional code> <DEST register> <OPERAND register 1> <OPERAND register 2> (Compare the two operands and store the result in dest. 1 == true, 0 == false. Codes include [EQ | LT | GT | LE | GE | NE])
    nullptr,                // 0b00110110 - empty
    nullptr,                // 0b00110111 - empty
    nullptr,                // 0b00111000 - empty
    nullptr,                // 0b00111001 - empty
    nullptr,                // 0b00111010 - empty
    nullptr,                // 0b00111011 - empty
    nullptr,                // 0b00111100 - empty
    nullptr,                // 0b00111101 - empty
    nullptr,                // 0b00111110 - empty
    nullptr,                // 0b00111111 - empty
    _decode_instr_20bimm,   // 0b00110111 - CALL_REL <20 bit signed immediate> (Push the PC to the stach, then jump to the immediate relative to the current PC value.)
    _decode_instr_reg_imm,  // 0b00111000 - CALL_IMM <BASE_ADDR register> <20 byte immediate> (Push the PC to the stack, then jump to the function specified at the address + the immediate offset)
    _decode_instr_2reg,     // 0b00111001 - CALL <BASE_ADDR register> <OFFSET register> (Push the PC to the stack, then jump to the address in the address register + the offset register)
    _decode_instr_only,     // 0b00111010 - RET (Pop the PC from the stack)
    _decode_instr_1reg,     // 0b00111011 - PUSH <SRC register> (Push a provided register to the stack)
    _decode_instr_1reg,     // 0b00111100 - POP <DEST register> (Pop the entry on the stack into the provided register)
    _decode_instr_1reg,     // 0b00111101 - PUSH_EXT <SRC EXT register> (Push an extended register to the stack)
    _decode_instr_1reg,     // 0b00111110 - POP_EXT <DEST EXT register> (Pop the entry on the stack into the extended register)
    _decode_instr_1reg,     // 0b01000000 - SYSCALL <ID register> (Push the PC to the stack, switch to Kernel mode, then jump to the registered syscall handler)
    _decode_instr_only,     // 0b01000001 - $P0 ONLY$ - SYSRET (Switch to User mode and pop the PC from the stack)
    _decode_instr_only,     // 0b01000010 - $P0 ONLY$ - HLT (Halt the CPU. May be unhalted by any active interrupts)
    _decode_instr_1reg      // 0b01000011 - CPUID <DEST register> (Get the 32 bit CPU ID info and store it in the dest register)
};

// Perform no decoding work. Only the instruction is necessary.
DecodedInstruction* _decode_instr_only(uint32_t instr) {
    return new DecodedInstruction {
        nullptr,    // No operands; don't allocate an array.
        0,          // No operands.
        0,          // Instruction value can be populated by the caller, which already has the instruction value.
    };
}

// Decode one 4-bit value (called a register since register names are 4 bits)
DecodedInstruction* _decode_instr_1reg(uint32_t instr) {
    uint32_t* operands = new uint32_t[1];
    operands[0] = instr >> 20 & 0xF;

    return new DecodedInstruction {
        operands,
        1,          // One operand.
        0,          // Instruction value can be populated by the caller, which already has the instruction value.
    };
}

DecodedInstruction* _decode_instr_2reg(uint32_t instr);
DecodedInstruction* _decode_instr_3reg(uint32_t instr);
DecodedInstruction* _decode_instr_4reg(uint32_t instr);
DecodedInstruction* _decode_instr_20bimm(uint32_t instr);
DecodedInstruction* _decode_instr_reg_imm(uint32_t instr);
DecodedInstruction* _decode_instr_reg_16bimm(uint32_t instr);
}