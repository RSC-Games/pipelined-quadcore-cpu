ISA_VERSION_MAJOR = 1
ISA_VERSION_MINOR = 0
ISA_VERSION = f"v{ISA_VERSION_MAJOR}.{ISA_VERSION_MINOR}"

INSTRUCTION_LUT = {
    "add": 0b00000001,          # ADD SRC_REG1 SRC_REG2 DEST_REG
    "add_acc": 0b00000010,      # ADD SRC_REG1/DEST_REG SRC_REG2 (add accumulate)
    "sub": 0b000000011,         # SUB SRC_REG1 SRC_REG2 DEST_REG
    "sub_acc": 0b00000100,      # SUB SRC_REG1/DEST_REG SRC_REG2 (sub accumulate)
    "mul": 0b00000101,          # MUL SRC_REG1 SRC_REG2 DEST_REG
    "mul_acc": 0b00000110,      # MUL SRC_REG1/DEST_REG SRC_REG2 (mul accumulate, not FMA!)
    "div": 0b00000111,          # DIV SRC_REG1 SRC_REG2 DEST_REG
    "div_acc": 0b00001000,      # DIV SRC_REG1/DEST_REG SRC_REG2 (mul accumulate, not FMA!)
                                # NOT IMPLEMENTED: ADD_FP
                                # NOT IMPLEMENTED: SUB_FP
                                # NOT IMPLEMENTED: MUL_FP
                                # NOT IMPLEMENTED: DIV_FP
    "mov": 0b00001101,          # MOV SRC_REG DEST_REG
    "mov_imm": 0b00001110,      # MOV DEST_REG IMM_20_BIT
    "mov_imm_high": 0b00001111, # MOVH DEST_REG IMM_16_BIT (MOV HIGH 16 BITS)
    "ldb": 0b00010000,          # LDB ADDR_REG DEST_REG IMM_16_BIT_OFFSET
    "stb": 0b00010001,          # STB ADDR_REG SRC_REG IMM_16_BIT_OFFSET
    "ld": 0b00010010,           # LD ADDR_REG DEST_REG
    "st": 0b00010011,           # ST ADDR_REG SRC_REG
    "and": 0b00010100,          # AND SRC_REG1 SRC_REG2 DEST_REG
    "or": 0b00010101,           # OR SRC_REG1 SRC_REG2 DEST_REG
    "xor": 0b00010110,          # XOR SRC_REG1 SRC_REG2 DEST_REG
    "not": 0b00010111,          # NOT SRC_REG DEST_REG
    "lsh": 0b00011000,          # LSH OPERAND_REG SHIFT_REG
    "lsh_imm": 0b00011001,      # LSH OPERAND_REG IMM_20_BIT_SHIFT
    "rsh": 0b00011010,          # RSH OPERAND_REG SHIFT_REG
    "rsh_imm": 0b00011011,      # RSH OPERAND_REG IMM_20_BIT_SHIFT
    "jmp_imm": 0b00011100,      # JMP ADDR_REG IMM_20_BIT_SHIFT
    "jmp_rel": 0b00011101,      # JMP BASE_ADDR_REG OFFSET_REG
    "jmp": 0b00011110,          # JMP ADDR_REG
    "jmpc": 0b00011111,         # JMP COND OPERAND_REG1 OPERAND_REG2 ADDR_REG
    "jmpc_z": 0b00100000,       # JMPZ COND OPERAND_REG1 ADDR_REG
    "call_imm": 0b00100001,     # CALL ADDR_REG IMM_20_BIT_OFFSET
    "call": 0b00100010,         # CALL BASE_ADDR_REG OFFSET_REG
    "ret": 0b00100011,          # RET
    "push": 0b00100100,         # PUSH REG
    "pop": 0b00100101,          # POP REG
    "hlt": 0b00100110,          # HLT
    "syscall": 0b00100111,      # SYSCALL ID_REG
    "sysret": 0b00101000,       # SYSRET
    "systarget": 0b00101001,    # SYSTARGET HANDLER_ADDRESS_REG
    "cpuid": 0b00101010         # CPUID DEST_REG
}