from .optypes import OPTYPE_CONDITIONAL, OPTYPE_IMMEDIATE, OPTYPE_REGISTER
from .optypes import REVERSE_OP_LUT

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
    ("ret",): "ret",                                                             # RET
    ("push", OPTYPE_REGISTER): "push",                                          # PUSH REG
    ("pop", OPTYPE_REGISTER): "pop",                                            # POP REG
    ("hlt",): "hlt",                                                             # HLT
    ("syscall", OPTYPE_REGISTER): "syscall",                                    # SYSCALL ID_REG
    ("sysret"): "sysret",                                                       # SYSRET
    ("systarget", OPTYPE_REGISTER): "systarget",    # DEPRECATED DEPRECATED #   # SYSTARGET HANDLER_ADDRESS_REG
    ("cpuid", OPTYPE_REGISTER): "cpuid"                                         # CPUID DEST_REG
}

INSTR_REENCODE_LUT = {
    "nop": ("lsh", "X0", "0x00")
}


def instruction_def_exists(instr_name: str) -> bool:
    valid_names = [name[0] for name in INSTR_ALIAS_LUT]
    return instr_name in valid_names


def print_instruction_entries(instr_name: str) -> None:
    entries = [entry for entry in INSTR_ALIAS_LUT if entry[0] == instr_name]

    for entry in entries:
        print(f"\t{_entry_as_str(entry)}")  # type: ignore


def _entry_as_str(entry: tuple) -> str:
    out_str = entry[0]
    ops = " ".join([REVERSE_OP_LUT[op] for op in entry[1:]])
    return f"{out_str} {ops}"


def operands_as_str(operands: list) -> str:
    return ", ".join([REVERSE_OP_LUT[op.type] for op in operands])


def _encode_instr_only(instr) -> int:
    word = 0 & 0xFFFFFFFF
    word |= (instr & 0xFFFFFFFF) << 24
    return word; 


# Use loop?
def _encode_instr_4reg(instr, reg0, reg1, reg2, reg3) -> int:
    word = _encode_instr_only(instr)
    word |= (reg0.encoded & 0xF) << 20
    word |= (reg1.encoded & 0xF) << 16
    word |= (reg2.encoded & 0xF) << 12
    word |= (reg3.encoded & 0xF) << 8
    return word


def _encode_instr_3reg(instr, reg0, reg1, reg2) -> int:
    word = _encode_instr_only(instr)
    word |= (reg0.encoded & 0xF) << 20
    word |= (reg1.encoded & 0xF) << 16
    word |= (reg2.encoded & 0xF) << 12
    return word


def _encode_instr_2reg(instr, reg0, reg1) -> int:
    word = _encode_instr_only(instr)
    word |= (reg0.encoded & 0xF) << 20
    word |= (reg1.encoded & 0xF) << 16
    return word


def _encode_instr_1reg(instr, reg) -> int:
    word = _encode_instr_only(instr)
    word |= (reg.encoded & 0xF) << 20
    return word


def _encode_instr_reg_imm(instr, reg, imm_20b) -> int:
    word = _encode_instr_only(instr)
    word |= (reg.encoded & 0xF) << 20
    word |= (imm_20b.encoded & 0xFFFFFFFF)
    return word


def _encode_instr_2reg_imm(instr, reg0, reg1, imm_16b) -> int:
    word = _encode_instr_only(instr)
    word |= (reg0.encoded & 0xF) << 20
    word |= (reg1.encoded & 0xF) << 16
    word |= (imm_16b.encoded & 0xFFFFFFFF)
    return word


def encode_word(instr, operands: list) -> int:
    """
    Implements the encoding behavior dictated in isa_design.txt
    
    -- EXERPT --
    Instruction decode types:
        INSTR_ONLY: (no operands).
            - ADD_FP, SUB_FP, MUL_FP, DIV_FP, RET, HLT, SYSRET
        INSTR_4REG: (4 register operands; technically just 4-bit operands at the hw level).
            - JMPC
        INSTR_3REG: (3 register operands).
            - ADD, SUB, MUL, DIV, AND, OR, XOR, JMPC_Z
        INSTR_2REG: (2 register operands).
            - ADD_ACC, SUB_ACC, MUL_ACC, DIV_ACC, MOV, NOT, LSH, RSH, JMP_REL, CALL
        INSTR_1REG: (1 register operand).
            - JMP, PUSH, POP, SYSCALL, SYSTARGET, CPUID
        INSTR_REG_IMM: (1 register, 1 20-bit immediate).
            - MOV_IMM, LSH_IMM, RSH_IMM, JMP_IMM, CALL_IMM
        INSTR_2REG_IMM: (2 register, 1 16-bit immediate).
            - LDB, STB, LD, ST, MOV_IMM_HIGH
    -- EXERPT --

    :param instr: Instruction-class object to operate on and encode.
    :param operands: Operands to encode.
    """

    encoder_lookup = {
        "add": _encode_instr_3reg,
        "add_acc": _encode_instr_2reg,      # ADD SRC_REG1/DEST_REG SRC_REG2 (add accumulate)
        "sub": _encode_instr_3reg,          # SUB SRC_REG1 SRC_REG2 DEST_REG
        "sub_acc": _encode_instr_2reg,      # SUB SRC_REG1/DEST_REG SRC_REG2 (sub accumulate)
        "mul": _encode_instr_3reg,          # MUL SRC_REG1 SRC_REG2 DEST_REG
        "mul_acc": _encode_instr_2reg,      # MUL SRC_REG1/DEST_REG SRC_REG2 (mul accumulate, not FMA!)
        "div": _encode_instr_3reg,          # DIV SRC_REG1 SRC_REG2 DEST_REG
        "div_acc": _encode_instr_2reg,      # DIV SRC_REG1/DEST_REG SRC_REG2 (mul accumulate, not FMA!)
                                            # NOT IMPLEMENTED: ADD_FP
                                            # NOT IMPLEMENTED: SUB_FP
                                            # NOT IMPLEMENTED: MUL_FP
                                            # NOT IMPLEMENTED: DIV_FP
        "mov": _encode_instr_2reg,          # MOV SRC_REG DEST_REG
        "mov_imm": _encode_instr_reg_imm,   # MOV DEST_REG IMM_20_BIT
        "mov_imm_high": _encode_instr_2reg_imm, # MOVH DEST_REG IMM_16_BIT (MOV HIGH 16 BITS)
        "ldb": _encode_instr_2reg_imm,      # LDB ADDR_REG DEST_REG IMM_16_BIT_OFFSET
        "stb": _encode_instr_2reg_imm,      # STB ADDR_REG SRC_REG IMM_16_BIT_OFFSET
        "ld": _encode_instr_2reg,           # LD ADDR_REG DEST_REG
        "st": _encode_instr_2reg,           # ST ADDR_REG SRC_REG
        "and": _encode_instr_3reg,          # AND SRC_REG1 SRC_REG2 DEST_REG
        "or": _encode_instr_3reg,           # OR SRC_REG1 SRC_REG2 DEST_REG
        "xor": _encode_instr_3reg,          # XOR SRC_REG1 SRC_REG2 DEST_REG
        "not": _encode_instr_2reg,          # NOT SRC_REG DEST_REG
        "lsh": _encode_instr_2reg,          # LSH OPERAND_REG SHIFT_REG
        "lsh_imm": _encode_instr_reg_imm,   # LSH OPERAND_REG IMM_20_BIT_SHIFT
        "rsh": _encode_instr_2reg,          # RSH OPERAND_REG SHIFT_REG
        "rsh_imm": _encode_instr_reg_imm,   # RSH OPERAND_REG IMM_20_BIT_SHIFT
        "jmp_imm": _encode_instr_reg_imm,   # JMP ADDR_REG IMM_20_BIT_SHIFT
        "jmp_rel": _encode_instr_2reg,      # JMP BASE_ADDR_REG OFFSET_REG
        "jmp": _encode_instr_1reg,          # JMP ADDR_REG
        "jmpc": _encode_instr_4reg,         # JMP COND OPERAND_REG1 OPERAND_REG2 ADDR_REG
        "jmpc_z": _encode_instr_3reg,       # JMPZ COND OPERAND_REG1 ADDR_REG
        "call_imm": _encode_instr_reg_imm,  # CALL ADDR_REG IMM_20_BIT_OFFSET
        "call": _encode_instr_2reg,         # CALL BASE_ADDR_REG OFFSET_REG
        "ret": _encode_instr_only,          # RET
        "push": _encode_instr_1reg,         # PUSH REG
        "pop": _encode_instr_1reg,          # POP REG
        "hlt": _encode_instr_only,          # HLT
        "syscall": _encode_instr_1reg,      # SYSCALL ID_REG
        "sysret": _encode_instr_only,       # SYSRET
        "systarget": _encode_instr_1reg,    # SYSTARGET HANDLER_ADDRESS_REG
        "cpuid": _encode_instr_1reg         # CPUID DEST_REG
    }

    encoder = encoder_lookup[instr.instr]
    return encoder(instr.opcode, *operands)