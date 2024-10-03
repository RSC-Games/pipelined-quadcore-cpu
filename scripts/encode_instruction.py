import include.isa
import include.optypes

import sys


def main(argv: list[str]) -> int:
    instr = input("Enter instruction to encode (ENCODED IN ASSEMBLY). For help, please "
                  + "consult the CPU ISA documentation.\n> ")

    encoded = encode_instr(instr)
    print(encoded)
    return 0


def encode_instr(instr: str) -> int:
    """
    Encode a string-based instruction and return a 32 bit unsigned integer
    representing the instruction opcode.

    :param instr: Instruction in source form.
    :return int: 32 bit unsigned instruction opcode.
    """

    # Strip out any assembly comments (why they're there I have no clue).
    instr = instr.split(";")[0].strip()
    
    comps = instr.split(" ")
    print(comps)

    # Determine operand types
    op_types = instr_get_operand_types(comps)

    return 0


def instr_get_operand_types(comps: list[str]):
    return [get_comp_type(comp) for comp in comps]


def get_comp_type(comp: str) -> str:
    """
    Identify the type of an instruction component. Can be a register, immediate,
    label (not recognized currently), or conditional code.
    
    :param comp: The component to type-identify.
    :return type: Operand type.
    """

    


if __name__ == "__main__":
    sys.exit(main(sys.argv))