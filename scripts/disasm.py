from include import log, isa_decode, optypes

import sys
import os
import io


def read_elf_headers():
    # TODO: Implement ELF file decoding.
    """
    See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format for more information.

    Decode an ELF header (bounds inclusive).
    0x00-0x03: Magic: 0x7F (ELF: 0x454C46)
    0x04: Class: 0x1 (32 bit binary)
    0x05: Endianness: 0x1 (little endian)
    0x06: ELF Header Version: 0x1
    0x07: OS ABI: 0xFF (No generic value; stealing one)
    0x08: ABI Version: 0x1 (Assuming first version)
    0x09: Padding: 0x0
    0x10-0x11: File type: 0x04 (Core file)
    0x12-0x13: Machine type: 0x110 (No value assigned, stealing it).
    0x14-0x17: ELF file version: 0x1
    0x18-0x1B: Entry point address (in memory).
    0x1C-0x1F: Program Header table offset (generally 0x34)
    0x20-0x23: Section Header offset 
    0x24-0x27: Flags (unused on x64, probably will remain unused in my ISA).
    0x28-0x29: ELF Header size: 0x34
    0x2A-0x2B: Program Header table entry size (0x20)
    0x2C-0x2D: Program Header table entries
    0x2E-0x2F: Section Header table entry size (0x28)
    0x30-0x31: Section Header table entries
    0x32-0x33: Section Header table entry index w/ section names.
    0x34 (END OF HEADER -- START OF PROGRAM HEADER TABLE)

    NOTE: Looks like the program header is not important for a disassembler.
    Program Header Table entry:
    0x00-0x03: Program Segment type (See the wiki page)
    0x04-0x07: Segment offset in this file.
    0x08-0x0B: Segment virtual address start offset
    0x0C-0x0F: Segment physical address start offset (only relevant for some systems).
    0x10-0x13: Segment size in the file
    0x14-0x17: Segment size in memory
    0x18-0x1B: Flags (see wiki -- executable | writable | readable)
    0x1C-0x1F: Segment alignment (power of 2; this arch is 4 byte-aligned)
    0x20 (END OF PROGRAM HEADER ENTRY)

    NOTE: Section header is important to decode.
    Section Header Table entry:
    0x00-0x03: Offset in .shstrtab for a string for this section's name.
    0x04-0x07: Section type.
    0x08-0x0B: Section flags.
    0x0C-0x0F: Section Virtual address (for sections that are dynamically loaded)
    0x10-0x13: Offset of this section in the file.
    0x14-0x17: Section size in bytes.
    0x18-0x1B: sh_link (?)
    0x1C-0x1F: Section Info (?)
    0x20-0x23: Section alignment (this arch is 4 byte-aligned)
    0x24-0x27: Size of an entry for sections with fixed-size entries.
    0x28: (END OF SECTION HEADER ENTRY) 
    """
    pass


def decode_bin(binary: io.BufferedReader):
    """
    Decode a flat binary. Not recommended as flat binaries may have their
    .data, .rodata, and .bss segments interpreted as executable code.
    Only useful for a bootloader and testing the emulator.
    """

    binsize = os.path.getsize(binary.name)
    print("ENTRY POINT:")

    while binary.tell() < binsize:
        offset = binary.tell()

        instruction = int.from_bytes(binary.read(4), 'little', signed=False)
        asm = decode_instruction(instruction)

        print(f"{hex(offset)}:\t\t{asm}")

    print("End of File.")


def decode_instruction(instruction: int) -> str:
    # Get the instruction opcode first, since that will tell us how it
    # needs to be decoded.
    opcode = instruction >> 24 & 0xFF
    instr_name = isa_decode.DECODE_LUT[opcode]
    
    # Decode the operands.
    raw_values = isa_decode.decode_word(instr_name, instruction)
    decoded = isa_decode.decode_optypes(isa_decode.REVERSE_ALIAS_LUT[instr_name], raw_values)
    
    return f"{instr_name} {' '.join(decoded)}"
        

def main(argv: list[str]) -> int:
    _ = argv[0] # Python file here.
    fpath = argv[1]

    print(f"Disassembling binary {fpath}")
    binary = open(fpath, "rb")

    # Identify an ELF or a raw binary (.elf or .bin)
    ftype = os.path.splitext(fpath)[1]

    if ftype == ".elf":
        log.print_error("ELF decoding is not implemented!")
        return -1
    elif ftype == ".bin":
        decode_bin(binary)
    else:
        log.print_error(f"Unsupported file type: {ftype}")

    return 0


if __name__ == "__main__":
    main(sys.argv)