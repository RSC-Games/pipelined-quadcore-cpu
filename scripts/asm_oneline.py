from include import isa, optypes
import binascii
import sys


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
    instr_name = comps[0].lower()

    # In special cases, one instruction is an alias of another. Find these cases.
    if instr_name in isa.INSTR_REENCODE_LUT:
        instr = " ".join(isa.INSTR_REENCODE_LUT[instr_name])
        comps = instr.split(" ")
        instr_name = comps[0].lower()

    # Determine operand types
    op_types = instr_get_operand_types(comps)

    if op_types is None:
        return -1

    instr_opcode = encode_instr_opcode(instr_name, op_types)

    if instr_opcode is None:
        if isa.instruction_def_exists(instr_name):
            print(f"ERROR: Unrecognized instruction/operand pair: {instr_name}, {isa.operands_as_str(op_types)}")
            print("Valid instruction definitions listed below:")
            isa.print_instruction_entries(instr_name)

        else:
            print(f"ERROR: Undefined instruction: {instr_name}")
        return -1

    # Encoded in little endian form.
    return isa.encode_word(instr_opcode, op_types)


def instr_get_operand_types(comps: list[str]) -> list[isa.InstructionComponent] | None:
    try:
        return [get_comp_type(comp) for comp in comps[1:]]
    except RuntimeError:
        return None


def get_comp_type(comp: str) -> isa.InstructionComponent:
    """
    Identify the type of an instruction component. Can be a register, immediate,
    label (not recognized currently), or conditional code.
    
    :param comp: The component to type-identify.
    :return type: Operand type.
    """

    # Is this a register ID or conditional?
    if comp in optypes.REGS_LIST:
        #print(f"Identified register {comp}")
        return isa.InstructionComponent(optypes.OPTYPE_REGISTER, comp, optypes.REGS_LIST[comp])
    elif comp in optypes.COND_LIST:
        #print(f"Identified conditional {comp}")
        return isa.InstructionComponent(optypes.OPTYPE_CONDITIONAL, comp, optypes.COND_LIST[comp])
    
    # TODO: Later add support for labels (in actual assembler)
    # Is this a hexadecimal value?
    if comp[:2] != "0x":
        print(f"ERROR: Unrecognized component {comp}")
        raise RuntimeError("Unrecognized component.")

    potential_hex = comp[2:]

    try:
        val = binascii.unhexlify(potential_hex)
        #print(f"Identified address {comp}")
        return isa.InstructionComponent(optypes.OPTYPE_IMMEDIATE, comp, int.from_bytes(val, 'big', signed=False))
    except:
        print(f"ERROR: Illegal hex value: {comp}")
        raise RuntimeError("Unrecognized component.")
    

def encode_instr_opcode(instr: str, optypes: list[isa.InstructionComponent]) -> isa.Instruction | None:
    #print(f"instr name {instr} optypes {optypes}")
    attribs = [instr]
    attribs.extend([op.type for op in optypes])  # type: ignore

    name = isa.INSTR_ALIAS_LUT.get(tuple(attribs), None)  # type: ignore

    if name is None:
        return None

    opcode = isa.INSTRUCTION_LUT[name]
    return isa.Instruction(name, opcode)
    

def main(argv: list[str]) -> int:
    while True:
        instr = input("Enter instruction to encode (ENCODED IN ASSEMBLY). For help, please "
                    + "consult the CPU ISA documentation.\n> ")

        encoded = encode_instr(instr)

        if encoded == -1:
            print()
            continue
        
        # Re-encode as little endian for display.
        dec = int.to_bytes(encoded, 4, byteorder='little', signed=False)
        encoded = int.from_bytes(dec, 'big', signed=False)

        print(f"Encoded result (little endian hex): {hex(encoded)}\n")

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))