namespace core {

const int REG_X0 = 0;
const int REG_X1 = 1;
const int REG_X2 = 2;
const int REG_X3 = 3;
const int REG_X4 = 4;
const int REG_X5 = 5;
const int REG_X6 = 6;
const int REG_X7 = 7;
const int REG_X8 = 8;
const int REG_X9 = 9;
const int REG_X10 = 10;
const int REG_X11 = 11;
const int REG_X12 = 12;

// X13 and XRV are the same register. They're just aliased.
const int REG_XRV = 13;
const int REG_X13 = 13;

// X14 and XFP are the same register (X_FRAME_PTR).
const int REG_XFP = 14;
const int REG_X14 = 14;

// X15 and XSP are the same register (X_STACK_PTR).
const int REG_XSP = 15;
const int REG_X15 = 15;

// CPU privilege modes.
const int PRIV_0_KERNEL_MODE = 0;
const int PRIV_1_USER_MODE = 1;

// Representation of an individual CPU core.
class Core {
    unsigned int priv_lvl = 0; // Current CPU core privilege level.
    unsigned int pc = 0x0;  // Current instruction to execute.
    unsigned int* register_file;  // Local architectural state.
    bool halted = true;
    // TODO: Memory Mangement Unit + Memory Protection Unit.
    // TODO: Manage pipeline.

    public:
    Core();
    Core(Core&& other) = delete;
    Core(const Core& other) = delete;
    ~Core();

    // Allow the SoC to control this and other core power states.
    void CORE_set_halt_state(bool halt);

    private:
    //void _empty  // NOP implemented as ADD_ACC r0, 0
    void op_add(int reg_src1, int reg_src2, int reg_dest);
    void op_add_acc(int reg_src_dest, int reg_src2);
    void op_sub(int reg_src1, int reg_src2, int reg_dest);
    void op_sub_acc(int reg_src_dest, int reg_src2);
    void op_mul(int reg_src1, int reg_src2, int reg_dest);
    void op_mul_acc(int reg_src_dest, int reg_src2);
    void op_div(int reg_src1, int reg_src2, int reg_dest);
    void op_div_acc(int reg_src_dest, int reg_src2);
    void _empty_add_fp();
    void _empty_sub_fp();
    void _empty_mul_fp();
    void _empty_div_fp();
    void op_mov(int reg_src, int reg_dest);
    void op_mov_imm(int reg_dest, unsigned int imm);
    void op_mov_imm_high(int reg_dest, unsigned short imm);
    void op_ldb(int reg_addr, int reg_dest, unsigned short imm_shift);
    void op_stb(int reg_addr, int reg_src, unsigned short imm_shift);
    void op_ld(int reg_addr, int reg_dest);
    void op_st(int reg_addr, int reg_src);
    void op_and(int reg_src1, int reg_src2, int reg_dest);
    void op_or(int reg_src1, int reg_src2, int reg_dest);
    void op_xor(int reg_src1, int reg_src2, int reg_dest);
    void op_not(int reg_src, int reg_dest);
    void op_lsh(int reg_operand, int reg_shift);
    void op_lsh_imm(int reg_operand, unsigned int imm_shift);
    void op_rsh(int reg_operand, int reg_shift);
    void op_rsh_imm(int reg_operand, unsigned int imm_shift);
    void op_jmp_imm(int reg_addr, unsigned int imm_offset);
    void op_jmp_rel(int reg_addr, int reg_offset);
    void op_jmp(int reg_addr);
    void op_jmpc(unsigned short cond_code, int reg_operand1, int reg_operand2, int reg_addr);
    void op_jmpc_z(bool cond_code, int reg_operand, int reg_addr);
    void op_call_imm(int reg_addr, unsigned int imm_offset);
    void op_call(int reg_addr, int reg_offset);
    void op_ret();
    void op_push(int reg_src);
    void op_pop(int reg_dest);
    void op_hlt();
    void op_syscall(int reg_id);
    void op_sysret();
    void op_systarget(int reg_addr);
};

}