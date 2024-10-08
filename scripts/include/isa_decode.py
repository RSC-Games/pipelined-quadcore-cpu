from .optypes import OPTYPE_COMP_CONDITIONAL, OPTYPE_ZERO_CONDITIONAL, OPTYPE_IMMEDIATE, OPTYPE_REGISTER_GP, OPTYPE_REGISTER_EXT
from . import optypes
from . import isa
import binascii

ISA_VERSION_MAJOR = 1
ISA_VERSION_MINOR = 1
ISA_VERSION = f"v{ISA_VERSION_MAJOR}.{ISA_VERSION_MINOR}"

DECODE_LUT = {isa.INSTRUCTION_LUT[key]: key for key in isa.INSTRUCTION_LUT}
REVERSE_ALIAS_LUT = {isa.INSTR_ALIAS_LUT[key]: key for key in isa.INSTR_ALIAS_LUT}

def _decode_instr_only(instr: int) -> list[int]:
    return []


def _decode_instr_5reg(instr: int) -> list[int]:
    return [
        instr >> 20 & 0xF, # REG0
        instr >> 16 & 0xF, # REG1
        instr >> 12 & 0xF, # REG2
        instr >> 8 & 0xF, # REG3
        instr >> 4 & 0xF, # REG4
    ]


def _decode_instr_4reg(instr: int) -> list[int]:
    return [
        instr >> 20 & 0xF, # REG0
        instr >> 16 & 0xF, # REG1
        instr >> 12 & 0xF, # REG2
        instr >> 8 & 0xF, # REG3
    ]


def _decode_instr_3reg(instr: int) -> list[int]:
    return [
        instr >> 20 & 0xF, # REG0
        instr >> 16 & 0xF, # REG1
        instr >> 12 & 0xF, # REG2
    ]


def _decode_instr_2reg(instr: int) -> list[int]:
    return [
        instr >> 20 & 0xF, # REG0
        instr >> 16 & 0xF, # REG1
    ]


def _decode_instr_1reg(instr: int) -> list[int]:
    return [
        instr >> 20 & 0xF, # REG
    ]


def _decode_instr_reg_imm(instr: int) -> list[int]:
    return [
        instr >> 20 & 0xF, # REG
        instr & 0xFFFFF, # IMM_20B
    ]


def _decode_instr_reg_16bimm(instr: int) -> list[int]:
    return [
        instr >> 20 & 0xF, # REG
        instr & 0xFFFF, # IMM_16B
    ]


def decode_word(instr_name: str, instr: int) -> list[int]:
    """
    Implements the decoding behavior dictated in isa_design.txt
    
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

    decoder_lookup = {
        "add": _decode_instr_2reg,          # ADD SRC_REG1/DEST_REG SRC_REG2
        "sub": _decode_instr_2reg,          # SUB SRC_REG1/DEST_REG SRC_REG2
        "mul": _decode_instr_2reg,          # MUL SRC_REG1/DEST_REG SRC_REG2
        "div": _decode_instr_2reg,          # DIV SRC_REG1/DEST_REG SRC_REG2
                                            # NOT IMPLEMENTED: ADD_FP
                                            # NOT IMPLEMENTED: SUB_FP
                                            # NOT IMPLEMENTED: MUL_FP
                                            # NOT IMPLEMENTED: DIV_FP
        "and": _decode_instr_2reg,          # AND SRC_REG1/DEST_REG SRC_REG2
        "or": _decode_instr_2reg,           # OR SRC_REG1/DEST_REG SRC_REG2 
        "xor": _decode_instr_2reg,          # XOR SRC_REG1/DEST_REG SRC_REG2
        "not": _decode_instr_1reg,          # NOT SRC_REG/DEST_REG

        "mov": _decode_instr_2reg,          # MOV DEST_REG SRC_REG
        "mov_imm": _decode_instr_reg_imm,   # MOV DEST_REG IMM_20_BIT
        "mov_imm_high": _decode_instr_reg_16bimm, # MOVH DEST_REG IMM_16_BIT (MOV HIGH 16 BITS)
        "mov_ext": _decode_instr_2reg,      # MOV DEST_EXT_REG SRC_REG
        "mov_gp": _decode_instr_2reg,       # MOV DEST_REG SRC_EXT_REG
        "mov_ext2": _decode_instr_2reg,     # MOV DEST_EXT_REG SRC_EXT_REG
        "mov_ext_imm": _decode_instr_reg_imm, # MOV DEST_EXT_REG IMM_20_BIT

        "ldb": _decode_instr_3reg,          # LDB DEST_REG BASE_ADDR_REG OFFSET_REG
        "stb": _decode_instr_3reg,          # STB SRC_REG BASE_ADDR_REG OFFSET_REG
        "ld": _decode_instr_3reg,           # LD DEST_REG BASE_ADDR_REG OFFSET_REG
        "st": _decode_instr_3reg,           # ST SRC_REG BASE_ADDR_REG OFFSET_REG

        "lsh": _decode_instr_2reg,          # LSH OPERAND_REG SHIFT_REG
        "lsh_imm": _decode_instr_reg_imm,   # LSH OPERAND_REG IMM_20_BIT_SHIFT
        "rsh": _decode_instr_2reg,          # RSH OPERAND_REG SHIFT_REG
        "rsh_imm": _decode_instr_reg_imm,   # RSH OPERAND_REG IMM_20_BIT_SHIFT

        "jmp_imm": _decode_instr_reg_imm,   # JMP ADDR_REG IMM_20_BIT_SHIFT
        "jmp_rel": _decode_instr_2reg,      # JMP BASE_ADDR_REG OFFSET_REG
        "jmp": _decode_instr_1reg,          # JMP ADDR_REG
        "jmpc": _decode_instr_5reg,         # JMP COND OPERAND_REG1 OPERAND_REG2 BASE_ADDR_REG OFFSET_REG
        "jmpc_z": _decode_instr_4reg,       # JMPZ COND OPERAND_REG1 BASE_ADDR_REG OFFSET_REG

        "call_imm": _decode_instr_reg_imm,  # CALL ADDR_REG IMM_20_BIT_OFFSET
        "call": _decode_instr_2reg,         # CALL BASE_ADDR_REG OFFSET_REG
        "ret": _decode_instr_only,          # RET
        "push": _decode_instr_1reg,         # PUSH REG
        "pop": _decode_instr_1reg,          # POP REG
        "push_ext": _decode_instr_1reg,     # PUSH EXT_REG
        "pop_ext": _decode_instr_1reg,      # POP EXT_REG

        "syscall": _decode_instr_1reg,      # SYSCALL ID_REG
        "sysret": _decode_instr_only,       # SYSRET
        "hlt": _decode_instr_only,          # HLT
        "cpuid": _decode_instr_1reg         # CPUID DEST_REG
    }

    decoder = decoder_lookup[instr_name]
    return decoder(instr)


def decode_optypes(in_types: tuple, values: list[int]) -> list[str]:
    out = []

    type_to_lut = {
        OPTYPE_REGISTER_GP: optypes.REVERSE_GP_REGS,
        OPTYPE_REGISTER_EXT: optypes.REVERSE_EXT_REGS,
        OPTYPE_COMP_CONDITIONAL: optypes.REVERSE_COMP_COND,
        OPTYPE_ZERO_CONDITIONAL: optypes.REVERSE_COMP_ZERO
    }

    for optype, value in zip(in_types[1:], values):
        lut = type_to_lut.get(optype)

        # Immediates must be decoded differently from the lookup-based keywords.
        if lut is None and optype == OPTYPE_IMMEDIATE:
            out.append(hex(value))
        elif lut is not None:
            out.append(lut[value])
        else:
            print("Should never reach this case! (isa_decode.py)")
            raise RuntimeError()

    return out