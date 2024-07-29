package core;

import mem.BOOTROM;
import mem.Memory;

/**
 * The CPU Core itself (with the 16-large register file.)
 */
public class Core {
    public static final int R0 = 0;
    public static final int R1 = 1;
    public static final int R2 = 2;
    public static final int R3 = 3;
    public static final int R4 = 4;
    public static final int R5 = 5;
    public static final int R6 = 6;
    public static final int R7 = 7;
    public static final int R8 = 8;
    public static final int R9 = 9;
    public static final int R10 = 10;
    public static final int R11 = 11;
    public static final int R12 = 12;

    // R13 and REV are the same, so they're aliased.
    public static final int REV = 13;
    public static final int R13 = 13;

    public static final int RFP = 14;
    public static final int R14 = 14;

    public static final int RSP = 15;
    public static final int R15 = 15;

    // CPU state registers.
    int[] registerFile;
    int pc;

    // Memory subsystem.
    BOOTROM rom;
    Memory mem;

    /**
     * Create a working core representation with a provided BOOTROM
     * and an available memory subsystem.
     * 
     * @param rom ROM
     * @param memory
     */
    public Core(BOOTROM rom, Memory memory) {
        this.rom = rom;
        this.mem = memory;
    }

    // TODO: Add instructions to execute to this class, and use the Instruction class
    // (and its subclasses) as a struct containing the additional decoded information
    // for execution of the instruction.
    void add(Add op) {}
    void add_acc(AddAcc op) {}
    void sub() {}
    void sub_acc() {}
    void mul() {}
    void mul_acc() {}
    void div() {}
    void div_acc() {}
    void mov() {}
    void mov_imm() {}
    void ldb() {}
    void stb() {}
    void ldhw() {}
    void sthw() {}
    void ld() {}
    void st() {}
    void and() {}
    void or() {}
    void xor() {}
    void not() {}
    void lsh() {}
    void lsh_imm() {}
    void rsh() {}
    void rsh_imm() {}
    void jmp_imm() {}
    void jmp_rel() {}
    void jmp() {}
    void jmpc() {}
    void jmpc_z() {}
    void call_imm() {}
    void call() {}
    void ret() {}
    void push() {}
    void pop() {}
    void hlt() {}
    void iow_imm() {}
    void iow() {}
    void iowb_imm() {}
    void iowb() {}
    void iowh_imm() {}
    void iowh() {}
    void ior() {}
    void iorb() {}
    void iorh() {}
}
