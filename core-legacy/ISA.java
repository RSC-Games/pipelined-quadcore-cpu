package core;

/**
 * Here lies the Instruction Set Architecture for the CPU. This contains
 * the hard-coded instruction constants for the CPU's encoded instructions.
 * For enhanced information on the instructions, see the ISA_DESIGN
 * specification this file is based on.
 */
public class ISA {
    public static final int ADD      = 0b00000001;
    public static final int ADD_ACC  = 0b00000010;
    public static final int SUB      = 0b00000011;
    public static final int SUB_ACC  = 0b00000100;
    public static final int MUL      = 0b00000101;
    public static final int MUL_ACC  = 0b00000110;
    public static final int DIV      = 0b00000111;
    public static final int DIV_ACC  = 0b00001000;
    public static final int MOV      = 0b00001001;
    public static final int MOV_IMM  = 0b00001010;
    public static final int LDB      = 0b00001011;
    public static final int STB      = 0b00001100;
    public static final int LDHW     = 0b00001101;
    public static final int STHW     = 0b00001110;
    public static final int LD       = 0b00001111;
    public static final int ST       = 0b00010000;
    public static final int AND      = 0b00010001;
    public static final int OR       = 0b00010010;
    public static final int XOR      = 0b00010011;
    public static final int NOT      = 0b00010100;
    public static final int LSH      = 0b00010101;
    public static final int LSH_IMM  = 0b00010110;
    public static final int RSH      = 0b00010111;
    public static final int RSH_IMM  = 0b00011000;
    public static final int JMP_IMM  = 0b00011001;
    public static final int JMP_REL  = 0b00011010;
    public static final int JMP      = 0b00011011;
    public static final int JMPC     = 0b00011100;
    public static final int JMPC_Z   = 0b00011101;
    public static final int CALL_IMM = 0b00011110;
    public static final int CALL     = 0b00011111;
    public static final int RET      = 0b00100000;
    public static final int PUSH     = 0b00100001;
    public static final int POP      = 0b00100010;
    public static final int HLT      = 0b00100011;
    public static final int IOW_IMM  = 0b00100100;
    public static final int IOW      = 0b00100101;
    public static final int IOWB_IMM = 0b00100110;
    public static final int IOWB     = 0b00100111;
    public static final int IOWH_IMM = 0b00101000;
    public static final int IOWH     = 0b00101001;
    public static final int IOR      = 0b00101010;
    public static final int IORB     = 0b00101011;
    public static final int IORH     = 0b00101100;
}
