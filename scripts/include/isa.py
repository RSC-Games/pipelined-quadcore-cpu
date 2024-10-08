from .optypes import OPTYPE_COMP_CONDITIONAL, OPTYPE_ZERO_CONDITIONAL, OPTYPE_IMMEDIATE, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_EXT
from . import optypes
from . import log
import binascii

ISA_VERSION_MAJOR = 1
ISA_VERSION_MINOR = 1
ISA_VERSION = f"v{ISA_VERSION_MAJOR}.{ISA_VERSION_MINOR}"

INSTRUCTION_LUT = {
    "add": 0b00000001,          # ADD SRC_REG1/DEST_REG SRC_REG2
    "sub": 0b00000010,          # SUB SRC_REG1/DEST_REG SRC_REG2
    "mul": 0b00000011,          # MUL SRC_REG1/DEST_REG SRC_REG2
    "div": 0b00000100,          # DIV SRC_REG1/DEST_REG SRC_REG2
                                # NOT IMPLEMENTED: ADD_FP
                                # NOT IMPLEMENTED: SUB_FP
                                # NOT IMPLEMENTED: MUL_FP
                                # NOT IMPLEMENTED: DIV_FP
    "and": 0b00010000,          # AND SRC_REG1/DEST_REG SRC_REG2
    "or": 0b00010001,           # OR SRC_REG1/DEST_REG SRC_REG2 
    "xor": 0b00010010,          # XOR SRC_REG1/DEST_REG SRC_REG2
    "not": 0b00010011,          # NOT SRC_REG/DEST_REG

    "mov": 0b00010100,          # MOV DEST_REG SRC_REG
    "mov_imm": 0b00010101,      # MOV DEST_REG IMM_20_BIT
    "mov_imm_high": 0b00010110, # MOVH DEST_REG IMM_16_BIT (MOV HIGH 16 BITS)
    "mov_ext": 0b00010111,      # MOV DEST_EXT_REG SRC_REG
    "mov_gp": 0b00011000,       # MOV DEST_REG SRC_EXT_REG
    "mov_ext2": 0b00011001,     # MOV DEST_EXT_REG SRC_EXT_REG

    "ldb": 0b00100000,          # LDB DEST_REG BASE_ADDR_REG OFFSET_REG
    "stb": 0b00100001,          # STB SRC_REG BASE_ADDR_REG OFFSET_REG
    "ld": 0b00100010,           # LD DEST_REG BASE_ADDR_REG OFFSET_REG
    "st": 0b00100011,           # ST SRC_REG BASE_ADDR_REG OFFSET_REG

    "lsh": 0b00101000,          # LSH OPERAND_REG SHIFT_REG
    "lsh_imm": 0b00101001,      # LSH OPERAND_REG IMM_20_BIT_SHIFT
    "rsh": 0b00101010,          # RSH OPERAND_REG SHIFT_REG
    "rsh_imm": 0b00101011,      # RSH OPERAND_REG IMM_20_BIT_SHIFT

    "jmp_imm": 0b00110000,      # JMP ADDR_REG IMM_20_BIT_SHIFT
    "jmp_rel": 0b00110001,      # JMP BASE_ADDR_REG OFFSET_REG
    "jmp": 0b00110010,          # JMP ADDR_REG
    "jmpc": 0b00110011,         # JMP COND OPERAND_REG1 OPERAND_REG2 BASE_ADDR_REG OFFSET_REG
    "jmpc_z": 0b00110100,       # JMPZ COND OPERAND_REG1 BASE_ADDR_REG OFFSET_REG

    "call_imm": 0b00111000,     # CALL ADDR_REG IMM_20_BIT_OFFSET
    "call": 0b00111001,         # CALL BASE_ADDR_REG OFFSET_REG
    "ret": 0b00111010,          # RET
    "push": 0b00111011,         # PUSH REG
    "pop": 0b00111100,          # POP REG
    "push_ext": 0b00111101,     # PUSH EXT_REG
    "pop_ext": 0b00111110,      # POP EXT_REG

    "syscall": 0b01000000,      # SYSCALL ID_REG
    "sysret": 0b01000001,       # SYSRET
    "hlt": 0b01000010,          # HLT
    "cpuid": 0b01000011         # CPUID DEST_REG
}

INSTR_ALIAS_LUT = {                                                          
    ("add", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "add",
    ("sub", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "sub",
    ("mul", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "mul",
    ("div", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "div",
    # NOT IMPLEMENTED: ADD_FP
    # NOT IMPLEMENTED: SUB_FP
    # NOT IMPLEMENTED: MUL_FP
    # NOT IMPLEMENTED: DIV_FP
    ("and", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "and",
    ("or", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "or",
    ("xor", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "xor",
    ("not", OPTYPE_REGISTER_GP): "not",

    ("mov", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "mov",
    ("mov", OPTYPE_REGISTER_GP, OPTYPE_IMMEDIATE): "mov_imm",
    ("movh", OPTYPE_REGISTER_GP, OPTYPE_IMMEDIATE): "mov_imm_high",
    ("mov", OPTYPE_REGISTER_EXT, OPTYPE_REGISTER_GP): "mov_ext",
    ("mov", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_EXT): "mov_gp",
    ("mov", OPTYPE_REGISTER_EXT, OPTYPE_REGISTER_EXT): "mov_ext2",

    ("ldb", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "ldb",
    ("stb", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "stb",
    ("ld", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "ld",
    ("st", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "st",

    ("lsh", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "lsh",
    ("lsh", OPTYPE_REGISTER_GP, OPTYPE_IMMEDIATE): "lsh_imm",
    ("rsh", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "rsh",
    ("rsh", OPTYPE_REGISTER_GP, OPTYPE_IMMEDIATE): "rsh_imm",

    ("jmp", OPTYPE_REGISTER_GP, OPTYPE_IMMEDIATE): "jmp_imm",
    ("jmp", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "jmp_rel",
    ("jmp", OPTYPE_REGISTER_GP): "jmp",
    ("jmpc", OPTYPE_COMP_CONDITIONAL, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "jmpc",
    ("jmpc", OPTYPE_ZERO_CONDITIONAL, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "jmpc_z",

    ("call", OPTYPE_REGISTER_GP, OPTYPE_IMMEDIATE): "call_imm",
    ("call", OPTYPE_REGISTER_GP, OPTYPE_REGISTER_GP): "call",
    ("ret",): "ret",
    ("push", OPTYPE_REGISTER_GP): "push",
    ("pop", OPTYPE_REGISTER_GP): "pop",
    ("push", OPTYPE_REGISTER_EXT): "push_ext",
    ("pop", OPTYPE_REGISTER_EXT): "pop_ext",

    ("syscall", OPTYPE_REGISTER_GP): "syscall",
    ("sysret",): "sysret",
    ("hlt",): "hlt",
    ("cpuid", OPTYPE_REGISTER_GP): "cpuid"
}

INSTR_REENCODE_LUT = {
    "nop": ("lsh", "X0", "0x00")
}

class InstructionComponent():
    def __init__(self, type: int, orig_val, encoded: int):
        self.type = type
        self.orig_val = orig_val
        self.encoded = encoded

    def __repr__(self):
        return f"[type: {optypes.REVERSE_OP_LUT[self.type]}, val: {self.orig_val}, hex: {hex(self.encoded)}]"


class Instruction():
    def __init__(self, instr: str, opcode: int):
        self.instr = instr
        self.opcode = opcode

    def __repr__(self):
        return f"[{self.instr}, 0x{binascii.hexlify(self.opcode.to_bytes(4, 'little'))}]"


def instruction_def_exists(instr_name: str) -> bool:
    valid_names = [name[0] for name in INSTR_ALIAS_LUT]
    return instr_name in valid_names


def print_instruction_entries(instr_name: str) -> None:
    entries = [entry for entry in INSTR_ALIAS_LUT if entry[0] == instr_name]

    for entry in entries:
        log.print_warnf(f"\t{_entry_as_str(entry)}")  # type: ignore


def _entry_as_str(entry: tuple) -> str:
    out_str = entry[0]
    ops = " ".join([optypes.REVERSE_OP_LUT[op] for op in entry[1:]])
    return f"{out_str} {ops}"


def operands_as_str(operands: list) -> str:
    return ", ".join([optypes.REVERSE_OP_LUT[op.type] for op in operands])


def _encode_instr_only(instr) -> int:
    word = 0 & 0xFFFFFFFF
    word |= (instr & 0xFFFFFFFF) << 24
    return word; 


def _encode_instr_5reg(instr, reg0, reg1, reg2, reg3, reg4) -> int:
    word = _encode_instr_only(instr)
    word |= (reg0.encoded & 0xF) << 20
    word |= (reg1.encoded & 0xF) << 16
    word |= (reg2.encoded & 0xF) << 12
    word |= (reg3.encoded & 0xF) << 8
    word |= (reg4.encoded & 0xF) << 8
    return word


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

    # TODO: Move to operand checking for the assembler.
    if imm_20b.encoded > 0xFFFFFF:
        log.print_warning("Warning! Immediate too large; truncating to 20 bits.")

    word |= (imm_20b.encoded & 0xFFFFFF)
    return word


def _encode_instr_reg_16bimm(instr, reg0, imm_16b) -> int:
    word = _encode_instr_only(instr)
    word |= (reg0.encoded & 0xF) << 20

    if imm_16b.encoded > 0xFFFF:
        log.print_warning("Warning! Immediate too large; truncating to 16 bits.")

    word |= (imm_16b.encoded & 0xFFFF)
    return word


def encode_word(instr, operands: list) -> int:
    """
    Implements the encoding behavior dictated in isa_design.txt
    
    -- EXERPT --
    Instruction decode types:
        INSTR_ONLY: (no operands).
            - ADD_FP, SUB_FP, MUL_FP, DIV_FP, RET, SYSRET, HLT
        INSTR_5REG: (5 register operands):
            - JMPC
        INSTR_4REG: (4 register operands; technically just 4-bit operands at the hw level).
            - JMPC_Z
        INSTR_3REG: (3 register operands).
            - LDB, STB, LD, ST
        INSTR_2REG: (2 register operands).
            - ADD, SUB, MUL, DIV, AND, OR, XOR, MOV, MOV_EXT, MOV_GP, MOV_EXT2, LSH, RSH, JMP_REL, CALL
        INSTR_1REG: (1 register operand).
            - NOT, JMP, PUSH, POP, PUSH_EXT, POP_EXT, SYSCALL, CPUID
        INSTR_REG_IMM: (1 register, 1 20-bit immediate).
            - MOV_IMM, LSH_IMM, RSH_IMM, JMP_IMM, CALL_IMM
        INSTR_2REG_IMM: (2 register, 1 16-bit immediate).
            - MOV_IMM_HIGH
    -- EXERPT --

    :param instr: Instruction-class object to operate on and encode.
    :param operands: Operands to encode.
    """

    encoder_lookup = {
        "add": _encode_instr_2reg,          # ADD SRC_REG1/DEST_REG SRC_REG2
        "sub": _encode_instr_2reg,          # SUB SRC_REG1/DEST_REG SRC_REG2
        "mul": _encode_instr_2reg,          # MUL SRC_REG1/DEST_REG SRC_REG2
        "div": _encode_instr_2reg,          # DIV SRC_REG1/DEST_REG SRC_REG2
                                            # NOT IMPLEMENTED: ADD_FP
                                            # NOT IMPLEMENTED: SUB_FP
                                            # NOT IMPLEMENTED: MUL_FP
                                            # NOT IMPLEMENTED: DIV_FP
        "and": _encode_instr_2reg,          # AND SRC_REG1/DEST_REG SRC_REG2
        "or": _encode_instr_2reg,           # OR SRC_REG1/DEST_REG SRC_REG2 
        "xor": _encode_instr_2reg,          # XOR SRC_REG1/DEST_REG SRC_REG2
        "not": _encode_instr_1reg,          # NOT SRC_REG/DEST_REG

        "mov": _encode_instr_2reg,          # MOV DEST_REG SRC_REG
        "mov_imm": _encode_instr_reg_imm,   # MOV DEST_REG IMM_20_BIT
        "mov_imm_high": _encode_instr_reg_16bimm, # MOVH DEST_REG IMM_16_BIT (MOV HIGH 16 BITS)
        "mov_ext": _encode_instr_2reg,      # MOV DEST_EXT_REG SRC_REG
        "mov_gp": _encode_instr_2reg,       # MOV DEST_REG SRC_EXT_REG
        "mov_ext2": _encode_instr_2reg,     # MOV DEST_EXT_REG SRC_EXT_REG

        "ldb": _encode_instr_3reg,          # LDB DEST_REG BASE_ADDR_REG OFFSET_REG
        "stb": _encode_instr_3reg,          # STB SRC_REG BASE_ADDR_REG OFFSET_REG
        "ld": _encode_instr_3reg,           # LD DEST_REG BASE_ADDR_REG OFFSET_REG
        "st": _encode_instr_3reg,           # ST SRC_REG BASE_ADDR_REG OFFSET_REG

        "lsh": _encode_instr_2reg,          # LSH OPERAND_REG SHIFT_REG
        "lsh_imm": _encode_instr_reg_imm,   # LSH OPERAND_REG IMM_20_BIT_SHIFT
        "rsh": _encode_instr_2reg,          # RSH OPERAND_REG SHIFT_REG
        "rsh_imm": _encode_instr_reg_imm,   # RSH OPERAND_REG IMM_20_BIT_SHIFT

        "jmp_imm": _encode_instr_reg_imm,   # JMP ADDR_REG IMM_20_BIT_SHIFT
        "jmp_rel": _encode_instr_2reg,      # JMP BASE_ADDR_REG OFFSET_REG
        "jmp": _encode_instr_1reg,          # JMP ADDR_REG
        "jmpc": _encode_instr_5reg,         # JMP COND OPERAND_REG1 OPERAND_REG2 BASE_ADDR_REG OFFSET_REG
        "jmpc_z": _encode_instr_4reg,       # JMPZ COND OPERAND_REG1 BASE_ADDR_REG OFFSET_REG

        "call_imm": _encode_instr_reg_imm,  # CALL ADDR_REG IMM_20_BIT_OFFSET
        "call": _encode_instr_2reg,         # CALL BASE_ADDR_REG OFFSET_REG
        "ret": _encode_instr_only,          # RET
        "push": _encode_instr_1reg,         # PUSH REG
        "pop": _encode_instr_1reg,          # POP REG
        "push_ext": _encode_instr_1reg,     # PUSH EXT_REG
        "pop_ext": _encode_instr_1reg,      # POP EXT_REG

        "syscall": _encode_instr_1reg,      # SYSCALL ID_REG
        "sysret": _encode_instr_only,       # SYSRET
        "hlt": _encode_instr_only,          # HLT
        "cpuid": _encode_instr_1reg         # CPUID DEST_REG
    }

    encoder = encoder_lookup[instr.instr]
    return encoder(instr.opcode, *operands)