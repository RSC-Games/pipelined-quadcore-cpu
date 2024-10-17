#include <cstdint>
#include "core/mmu.h"
#include "core/memory.h"
#include "common/isa_decode.h"

#ifndef INC_CORE_CPU_H
#define INC_CORE_CPU_H

namespace core {

// Register names and indexes.
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
    uint8_t priv_lvl = 0; // Current CPU core privilege level.
    uint32_t pc = 0x0;  // Current instruction to execute.
    uint32_t* register_file;  // Local architectural state.
    uint32_t clock_rate = 0;  // Current CPU clock rate.
    bool halted = true;

    core::MMU* mmu;
    // TODO: Memory Mangement Unit + Memory Protection Unit.
    // TODO: Manage pipeline.

    // Pipeline state
    uint32_t DECODE_next_instr;
    isa::DecodedInstruction* EXECUTE_decoded_instr;

public:
    Core(core::Memory* mem, core::BootROM* bootrom);
    Core(Core&& other) = delete;
    Core(const Core& other) = delete;
    ~Core();

    // Allow the SoC to control this and other core power states.
    void CORE_set_halt_state(bool halt);
    void CORE_set_clock_rate(uint32_t clock_rate_MHz);

    // Simulate the core and all core subsystems.
    void simulate();

private:
    // TODO: WHERE do I put these?
    void CORE_instr_fetch();
    void CORE_decode_instr();
    void CORE_execute_op();
    void CORE_write_back();

    // TODO: Implement pipeline and instruction decoder.
    void CORE_run_pipeline();

    // For debugging purposes. Print the exception type and PC.
    void print_exception_frame(uint32_t exception);

    // Erase all uncommitted instructions from the instruction pipeline.
    // Essential if a branch misprediction occurred.
    void CORE_flush_pipeline();

    // Predict the direction a branch will take.
    // NOTE: Branch prediction will be implemented via a 2-level predictor.
    // The last 8 branches will be stored. Each execute phase for a branch, the branch
    // history will be left shifted 1 bit, then a 1 or 0 will be XOR'd in depending
    // on the actual result of the prediction.
    // This value will be used to look up a 2 bit saturation counter value for the history
    // of that branch pattern.
    // Each branch instruction has its own saturation counter copy.
    inline void CORE_predict_branch();

    // TODO: Add branch target predictor.
    // NOTE: Branch target buffer required to prevent pipeline stalling immediately
    // after branch fetch. Implementation details:
    // Branch target buffer stores n entries.
    // Entries are indexed by PC and contain the last resolved branch address for that
    // instruction. If none exists, then stall the pipeline and store the target address
    // when the instruction is decoded.
    inline uint32_t CORE_predict_branch_target();

    inline uint32_t MMU_translate_addr(uint32_t vaddr);

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

#endif // INC_CORE_CPU_H