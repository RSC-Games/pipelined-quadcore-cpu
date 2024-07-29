package core;

/**
 * Generic instruction representation. Stores operands for execution.
 * One of the many subclasses will generally be passed to the core
 * for it to execute.
 */
public abstract class Instruction {
    // TODO: Implement the entire instruction hierarchy.
    // TODO: Perform instruction decoding within the actual
    // constructor itself instead of externally.
}

class Add extends Instruction {
    public final int srcReg1;
    public final int srcReg2;
    public final int destReg;

    public Add(int sr1, int sr2, int dr) {
        this.srcReg1 = sr1;
        this.srcReg2 = sr2;
        this.destReg = dr;
    }
}

class AddAcc extends Instruction {
    public final int srcDestReg1;
    public final int srcReg2;

    public AddAcc(int sdr, int sr2) {
        this.srcDestReg1 = sdr;
        this.srcReg2 = sr2;
    }
}

class Sub extends Instruction {
    public final int srcReg1;
    public final int srcReg2;
    public final int destReg;

    public Sub(int sr1, int sr2, int dr) {
        this.srcReg1 = sr1;
        this.srcReg2 = sr2;
        this.destReg = dr;
    }
}

class SubAcc extends Instruction {
    public final int srcDestReg1;
    public final int srcReg2;

    public SubAcc(int sdr, int sr2) {
        this.srcDestReg1 = sdr;
        this.srcReg2 = sr2;
    }
}

class Mul extends Instruction {
    public final int srcReg1;
    public final int srcReg2;
    public final int destReg;

    public Mul(int sr1, int sr2, int dr) {
        this.srcReg1 = sr1;
        this.srcReg2 = sr2;
        this.destReg = dr;
    }
}

class MulAcc extends Instruction {
    public final int srcDestReg1;
    public final int srcReg2;

    public MulAcc(int sdr, int sr2) {
        this.srcDestReg1 = sdr;
        this.srcReg2 = sr2;
    }
}

class Div extends Instruction {
    public final int srcReg1;
    public final int srcReg2;
    public final int destReg;

    public Div(int sr1, int sr2, int dr) {
        this.srcReg1 = sr1;
        this.srcReg2 = sr2;
        this.destReg = dr;
    }
}

class DivAcc extends Instruction {
    public final int srcDestReg1;
    public final int srcReg2;

    public DivAcc(int sdr, int sr2) {
        this.srcDestReg1 = sdr;
        this.srcReg2 = sr2;
    }
}

class Mov extends Instruction {
    public final int srcReg;
    public final int destReg;

    public Mov(int sr, int dr) {
        this.srcReg = sr;
        this.destReg = dr;
    }
}

class MovImm extends Instruction {
    public final int destReg;
    public final int imm;

    public MovImm(int dr, int imm) {
        this.destReg = dr;
        this.imm = imm;
    }
}

class Ldb extends Instruction {
    public final int addrReg;
    public final int destReg;

    public Ldb(int ar, int dr) {
        this.addrReg = ar;
        this.destReg = dr;
    }
}

class Stb extends Instruction {
    public final int addrReg;
    public final int srcReg;

    public Stb(int ar, int sr) {
        this.addrReg = ar;
        this.srcReg = sr;
    }
}

class Ldhw extends Instruction {
    public final int addrReg;
    public final int destReg;

    public Ldhw(int ar, int dr) {
        this.addrReg = ar;
        this.destReg = dr;
    }
}

class Sthw extends Instruction {
    public final int addrReg;
    public final int srcReg;

    public Sthw(int ar, int sr) {
        this.addrReg = ar;
        this.srcReg = sr;
    }
}

class Ld extends Instruction {
    public final int addrReg;
    public final int destReg;

    public Ld(int ar, int dr) {
        this.addrReg = ar;
        this.destReg = dr;
    }
}

class St extends Instruction {
    public final int addrReg;
    public final int srcReg;

    public St(int ar, int sr) {
        this.addrReg = ar;
        this.srcReg = sr;
    }
}

class And extends Instruction {
    public final int srcReg1;
    public final int srcReg2;
    public final int destReg;

    public And(int sr1, int sr2, int dr) {
        this.srcReg1 = sr1;
        this.srcReg2 = sr2;
        this.destReg = dr;
    }
}

class Or extends Instruction {
    public final int srcReg1;
    public final int srcReg2;
    public final int destReg;

    public Or(int sr1, int sr2, int dr) {
        this.srcReg1 = sr1;
        this.srcReg2 = sr2;
        this.destReg = dr;
    }
}

class Xor extends Instruction {
    public final int srcReg1;
    public final int srcReg2;
    public final int destReg;

    public Xor(int sr1, int sr2, int dr) {
        this.srcReg1 = sr1;
        this.srcReg2 = sr2;
        this.destReg = dr;
    }
}

class Not extends Instruction {
    public final int srcReg;
    public final int destReg;

    public Not(int sr, int dr) {
        this.srcReg = sr;
        this.destReg = dr;
    }
}

class Lsh extends Instruction {}

class LshImm extends Instruction {}

class Rsh extends Instruction {}
class RshImm extends Instruction {}
class JmpImm extends Instruction {}
class JmpRel extends Instruction {}
class Jmp extends Instruction {}
class Jmpc extends Instruction {} // TODO: MISSING OFFSET REGISTER!
class JmpcZ extends Instruction {} // TODO: MISSING OFFSET REGISTER!
class CallImm extends Instruction {} 
class call extends Instruction {}
class ret extends Instruction {}
class push extends Instruction {}
class pop extends Instruction {}
class hlt extends Instruction {}
class iow_imm extends Instruction {}
class iow extends Instruction {}
class iowb_imm extends Instruction {}
class iowb extends Instruction {}
class iowh_imm extends Instruction {}
class iowh extends Instruction {}
class ior extends Instruction {}
class iorb extends Instruction {}
class iorh extends Instruction {}
