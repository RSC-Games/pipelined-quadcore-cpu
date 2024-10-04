from include import isa, optypes
import binascii
import sys


class InstructionComponent():
    def __init__(self, type: int, data):
        self.type = type
        self.data = data


class Instruction():
    def __init__(self, instr: str, opcode: int):
        self.instr = instr
        self.opcode = opcode


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
    instr_opcode = encode_instr_opcode(instr, op_types)

    return 0


def instr_get_operand_types(comps: list[str]) -> list[InstructionComponent]:
    return [get_comp_type(comp) for comp in comps[1:]]


def get_comp_type(comp: str) -> InstructionComponent:
    """
    Identify the type of an instruction component. Can be a register, immediate,
    label (not recognized currently), or conditional code.
    
    :param comp: The component to type-identify.
    :return type: Operand type.
    """

    # Is this a register ID or conditional?
    if comp in optypes.REGS_LIST:
        print(f"Identified register {comp}")
        return InstructionComponent(optypes.OPTYPE_REGISTER, comp)
    elif comp in optypes.COND_LIST:
        print(f"Identified conditional {comp}")
        return InstructionComponent(optypes.OPTYPE_CONDITIONAL, comp)
    
    # Is this a hexadecimal value?
    potential_hex = comp[2:]

    try:
        _ = binascii.unhexlify(potential_hex)
        print(f"Identified address {comp}")
        return InstructionComponent(optypes.OPTYPE_IMMEDIATE, comp)
    except:
        print(f"ERROR: Unrecognized component {comp}")
        raise RuntimeError("Unrecognized component.")
    

def encode_instr_opcode(instr: str, optypes: list[InstructionComponent]) -> Instruction:
    # TODO: Take the instr and optypes and get the instruction opcode.
    pass #opcode = isa.
    

def main(argv: list[str]) -> int:
    instr = input("Enter instruction to encode (ENCODED IN ASSEMBLY). For help, please "
                  + "consult the CPU ISA documentation.\n> ")

    encoded = encode_instr(instr)
    print(encoded)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))