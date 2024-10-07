import sys


def read_bin_headers():
    # TODO: Implement ELF file decoding.
    """
    See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format for more information.

    Decode an ELF header.
    0x00-0x03: Magic: 0x7F (ELF: 0x454C46)
    0x04: Class: 0x1 (32 bit binary)
    0x05: Endianness: 0x1 (little endian)
    0x06: Version: 0x1
    0x07: OS ABI: 0xFF (No generic value; stealing one)
    0x08: ABI Version: 0x1 (Assuming first version)
    0x09: Padding: 0x0
    0x10: File type: 0x04 (Core file)
    0x11-0x12: Machine type: 0x110 (No value assigned, stealing it).
    """
    pass


def main(argv: list[str]) -> int:
    fpath = argv[0]

    print(f"Disassembling file: {fpath}")
    binary = open(fpath, "rb")

    return 0


if __name__ == "__main__":
    main(sys.argv)