from .optypes import OPTYPE_CONDITIONAL, OPTYPE_IMMEDIATE, OPTYPE_REGISTER

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

INSTR_ALIAS_LUT = {
    ("nop",): "lsh",                                                             # NOP                                                           
    ("add", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_REGISTER): "add",          # ADD SRC_REG1 SRC_REG2 DEST_REG
    ("add", OPTYPE_REGISTER, OPTYPE_REGISTER): "add_acc",                       # ADD SRC_REG1/DEST_REG SRC_REG2 (add accumulate)
    ("sub", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_REGISTER): "sub",          # SUB SRC_REG1 SRC_REG2 DEST_REG
    ("sub", OPTYPE_REGISTER, OPTYPE_REGISTER): "sub_acc",                       # SUB SRC_REG1/DEST_REG SRC_REG2 (sub accumulate)
    ("mul", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_REGISTER): "mul",          # MUL SRC_REG1 SRC_REG2 DEST_REG
    ("mul", OPTYPE_REGISTER, OPTYPE_REGISTER): "mul_acc",                       # MUL SRC_REG1/DEST_REG SRC_REG2 (mul accumulate, not FMA!)
    ("div", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_REGISTER): "div",          # DIV SRC_REG1 SRC_REG2 DEST_REG
    ("div", OPTYPE_REGISTER, OPTYPE_REGISTER): "div_acc",                       # DIV SRC_REG1/DEST_REG SRC_REG2 (mul accumulate, not FMA!)
                                                                                # NOT IMPLEMENTED: ADD_FP
                                                                                # NOT IMPLEMENTED: SUB_FP
                                                                                # NOT IMPLEMENTED: MUL_FP
                                                                                # NOT IMPLEMENTED: DIV_FP
    ("mov", OPTYPE_REGISTER, OPTYPE_REGISTER): "mov",                           # MOV SRC_REG DEST_REG
    ("mov", OPTYPE_REGISTER, OPTYPE_IMMEDIATE): "mov_imm",                      # MOV DEST_REG IMM_20_BIT
    ("mov_imm_high", OPTYPE_REGISTER, OPTYPE_IMMEDIATE): "mov_imm_high",        # MOVH DEST_REG IMM_16_BIT (MOV HIGH 16 BITS)
    ("ldb", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_IMMEDIATE): "ldb",         # LDB ADDR_REG DEST_REG IMM_16_BIT_OFFSET
    ("stb", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_IMMEDIATE): "stb",         # STB ADDR_REG SRC_REG IMM_16_BIT_OFFSET
    ("ld", OPTYPE_REGISTER, OPTYPE_REGISTER): "ld",                             # LD ADDR_REG DEST_REG
    ("st", OPTYPE_REGISTER, OPTYPE_REGISTER): "st",                             # ST ADDR_REG SRC_REG
    ("and", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_REGISTER): "and",          # AND SRC_REG1 SRC_REG2 DEST_REG
    ("or", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_REGISTER): "or",            # OR SRC_REG1 SRC_REG2 DEST_REG
    ("xor", OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_REGISTER): "xor",          # XOR SRC_REG1 SRC_REG2 DEST_REG
    ("not", OPTYPE_REGISTER, OPTYPE_REGISTER): "not",                           # NOT SRC_REG DEST_REG
    ("lsh", OPTYPE_REGISTER, OPTYPE_REGISTER): "lsh",                           # LSH OPERAND_REG SHIFT_REG
    ("lsh", OPTYPE_REGISTER, OPTYPE_IMMEDIATE): "lsh_imm",                      # LSH OPERAND_REG IMM_20_BIT_SHIFT
    ("rsh", OPTYPE_REGISTER, OPTYPE_REGISTER): "rsh",                           # RSH OPERAND_REG SHIFT_REG
    ("rsh", OPTYPE_REGISTER, OPTYPE_IMMEDIATE): "rsh_imm",                      # RSH OPERAND_REG IMM_20_BIT_SHIFT
    ("jmp", OPTYPE_REGISTER, OPTYPE_IMMEDIATE): "jmp_imm",                      # JMP ADDR_REG IMM_20_BIT_SHIFT
    ("jmp", OPTYPE_REGISTER, OPTYPE_REGISTER): "jmp_rel",                       # JMP BASE_ADDR_REG OFFSET_REG
    ("jmp", OPTYPE_REGISTER): "jmp",                                            # JMP ADDR_REG
    ("jmp", OPTYPE_CONDITIONAL, OPTYPE_REGISTER, OPTYPE_REGISTER, OPTYPE_REGISTER): "jmpc", # JMP COND OPERAND_REG1 OPERAND_REG2 ADDR_REG
    ("jmpz", OPTYPE_CONDITIONAL, OPTYPE_REGISTER, OPTYPE_REGISTER): "jmpc_z",   # JMPZ COND OPERAND_REG1 ADDR_REG
    ("call", OPTYPE_REGISTER, OPTYPE_IMMEDIATE): "call_imm",                    # CALL ADDR_REG IMM_20_BIT_OFFSET
    ("call", OPTYPE_REGISTER, OPTYPE_REGISTER): "call",                         # CALL BASE_ADDR_REG OFFSET_REG
    ("ret"): "ret",                                                             # RET
    ("push", OPTYPE_REGISTER): "push",                                          # PUSH REG
    ("pop", OPTYPE_REGISTER): "pop",                                            # POP REG
    ("hlt"): "hlt",                                                             # HLT
    ("syscall", OPTYPE_REGISTER): "syscall",                                    # SYSCALL ID_REG
    ("sysret"): "sysret",                                                       # SYSRET
    ("systarget", OPTYPE_REGISTER): "systarget",    # DEPRECATED DEPRECATED #   # SYSTARGET HANDLER_ADDRESS_REG
    ("cpuid", OPTYPE_REGISTER): "cpuid"                                         # CPUID DEST_REG
}

def encode_word():
    pass