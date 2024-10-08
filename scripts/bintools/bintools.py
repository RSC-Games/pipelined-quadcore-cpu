def write_raw(filename: str, instr_stream: list[int]):
    f = open(filename, "wb")
    
    for instr in instr_stream:
        f.write(int.to_bytes(instr, 4, 'little', signed=False))

    f.close()