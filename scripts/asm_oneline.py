from include import isa, optypes, log
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
            log.print_error(f"Unrecognized instruction/operand pair: {instr_name}, {isa.operands_as_str(op_types)}")
            log.print_warnf("Valid instruction definitions listed below:")
            isa.print_instruction_entries(instr_name)

        else:
            log.print_error(f"Unknown instruction: {instr_name}")
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

    # Is this a GP register ID, extended register, or conditional?
    if comp in optypes.GP_REGS_LIST:
        return isa.InstructionComponent(optypes.OPTYPE_REGISTER_GP, comp, optypes.GP_REGS_LIST[comp])
    elif comp in optypes.EXTENDED_REGS_LIST:
        return isa.InstructionComponent(optypes.OPTYPE_REGISTER_EXT, comp, optypes.EXTENDED_REGS_LIST[comp])
    elif comp in optypes.COMP_COND_LIST:
        return isa.InstructionComponent(optypes.OPTYPE_COMP_CONDITIONAL, comp, optypes.COMP_COND_LIST[comp])
    elif comp in optypes.ZERO_COND_LIST:
        return isa.InstructionComponent(optypes.OPTYPE_ZERO_CONDITIONAL, comp, optypes.ZERO_COND_LIST[comp])
    
    # TODO: Later add support for labels (in actual assembler)
    # Is this a hexadecimal value?
    if comp[:2] == "0x":
        potential_hex = comp[2:]

        try:
            val = binascii.unhexlify(potential_hex)
            #print(f"Identified address {comp}")
            return isa.InstructionComponent(optypes.OPTYPE_IMMEDIATE, comp, int.from_bytes(val, 'big', signed=False))
        except:

            log.print_error(f"Illegal hex value: {comp}")
            raise RuntimeError("Unrecognized component.")
        
    elif comp[:2] == "0b":
        bin_val = comp[2:]

        try:
            val = int(bin_val, 2)
            return isa.InstructionComponent(optypes.OPTYPE_IMMEDIATE, comp, val)
        except:
            log.print_error(f"Illegal bin value: {comp}")
            raise RuntimeError("Unrecognized component.")
    elif comp.isnumeric():
        return isa.InstructionComponent(optypes.OPTYPE_IMMEDIATE, comp, int(comp))
    else:
        log.print_error(f"Unrecognized component {comp}")
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

        log.print_ok(f"Encoded result (little endian hex): {hex(encoded)}\n")

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))