#include <cstdint>
#include "core/mmu.h"
#include "core/memory.h"
#include "common/isa_decode.h"

#ifndef INC_CORE_CPU_H
#define INC_CORE_CPU_H

namespace core {

// Register names and indexes.
// Extended registers are indexed the same as the gp regs (though
// ERV, EFP, and ESP don't exist).
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

struct PipelineStage;

// Representation of an individual CPU core.
class Core {
public:
    typedef void (Core::*instr_table_ptr)(PipelineStage&);
    typedef instr_table_ptr stage4_payload;
    typedef instr_table_ptr stage5_payload;

private:
    instr_table_ptr* instr_table;

    uint8_t priv_lvl = 0; // Current CPU core privilege level.
    uint32_t pc = 0x0;  // Current address to prefetch instruction.
    uint32_t* register_file;  // Local architectural state.
    uint32_t* extended_registers;  // Extended local architectural registers.
    uint32_t clock_rate = 0;  // Current CPU clock rate.
    bool halted = true;

    core::MMU* mmu;
    // TODO: Memory Mangement Unit + Memory Protection Unit.
    // TODO: Manage pipeline.

    // Pipeline state
    PipelineStage** pipeline_stages;

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
    void CORE_instr_fetch();
    void CORE_decode_instr();
    void CORE_execute_op();
    void CORE_mem_access();
    void CORE_commit_last_instr();

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

    inline instr_table_ptr resolve_instr_table_ptr(short instr_id);

    // Stage 3 payloads (execution)

    //void _empty  // NOP implemented as ADD_ACC r0, 0

    // Nop is not part of the ISA (only used for pipeline bubbling)
    void op_nop(PipelineStage& instr);
    void op_add(PipelineStage& instr);
    void op_sub(PipelineStage& instr);
    void op_mul(PipelineStage& instr);
    void op_div(PipelineStage& instr);
    void _empty_add_fp();
    void _empty_sub_fp();
    void _empty_mul_fp();
    void _empty_div_fp();
    void op_and(PipelineStage& instr);
    void op_or(PipelineStage& instr);
    void op_xor(PipelineStage& instr);
    void op_not(PipelineStage& instr);
    void op_mov(PipelineStage& instr);
    void op_mov_imm(PipelineStage& instr);
    void op_mov_imm_high(PipelineStage& instr);
    void op_mov_ext(PipelineStage& instr);
    void op_mov_gp(PipelineStage& instr);
    void op_mov_ext2(PipelineStage& instr);
    void op_mov_ext_imm(PipelineStage& instr);
    void op_ldb(PipelineStage& instr);
    void op_stb(PipelineStage& instr);
    void op_ld(PipelineStage& instr);
    void op_st(PipelineStage& instr);
    void op_lsh(PipelineStage& instr);
    void op_lsh_imm(PipelineStage& instr);
    void op_rsh(PipelineStage& instr);
    void op_rsh_imm(PipelineStage& instr);
    void op_jmp_imm(PipelineStage& instr);
    void op_jmp(PipelineStage& instr);
    void op_jmp_abs(PipelineStage& instr);
    void op_jmp_rel(PipelineStage& instr);
    void op_bnx(PipelineStage& instr);
    void op_cmp(PipelineStage& instr);
    void op_call_rel(PipelineStage& instr);
    void op_call_imm(PipelineStage& instr);
    void op_call(PipelineStage& instr);
    void op_ret(PipelineStage& instr);
    void op_push(PipelineStage& instr);
    void op_pop(PipelineStage& instr);
    void op_push_ext(PipelineStage& instr);
    void op_pop_ext(PipelineStage& instr);
    void op_syscall(PipelineStage& instr);
    void op_sysret(PipelineStage& instr);
    void op_hlt(PipelineStage& instr);
    void op_cpuid(PipelineStage& instr);

    // Pipeline stage 4 payloads (memory read/write)

    // Most instructions have nothing to do in stage 4.
    void stage4_nop(PipelineStage& instr);
    // Load a byte from mem_addr into reg_commit_val
    void stage4_load_byte(PipelineStage& instr);
    // Store a byte mem_data to mem_addr
    void stage4_store_byte(PipelineStage& instr);
    // Load a word from mem_addr into reg_commit_val
    void stage4_load_word(PipelineStage& instr);
    // Store a word mem_data to mem_addr 
    void stage4_store_word(PipelineStage& instr);

    // Pipeline stage 5 payloads (architectural commit)

    // Pipeline commit for invalidated instruction (only increments PC; doesn't touch the
    // stack pointer).
    void stage5_commit_invalid(PipelineStage& instr);
    // Stage 5 null commit increments the PC by 4 and commits nothing.
    void stage5_commit_null(PipelineStage& instr);
    // Commits a stored value to a general purpose register.
    void stage5_commit_gpreg(PipelineStage& instr);
    // Commits a stored value to an extended register.
    void stage5_commit_extreg(PipelineStage& instr);
    // Commit a new program counter value and continue execution from there. Does not increment
    // the PC.
    void stage5_commit_pc(PipelineStage& instr);
};

struct PipelineStage {
    // Program counter at instruction issuing (prevent prefetch PC from 
    // giving incorrect results).
    uint32_t pc_val;
    // Raw, undecoded instruction (obtained in stage 1)
    uint32_t instruction_bytes;
    // Decoded instruction information (arguments, etc; gotten in stage 2)
    isa::DecodedInstruction* decoded_instr;
    // Stage 3 payload (machine instruction to execute)
    Core::instr_table_ptr instr_func;
    // Stage 4 payload (memory i/o action)
    // Fully encoded instruction binary value.
    Core::stage4_payload stage4;
    // Stage 4 address to write the mem_data to.
    uint32_t mem_addr;
    // Data to commit to mem_addr in stage 4.
    uint32_t mem_data;
    // Stage 5 payload (commit to architectural state)
    Core::stage5_payload stage5;
    // GPREG/EXTREG id to commit to in stage 5
    uint32_t reg_id;
    // Register data to commit in stage 5 (if any)
    uint32_t reg_commit_val;
    // Alter the stack pointer. Typically zero except for specific instructions.
    // Since the stack grows down, positive values decrease it and negative increase it.
    int8_t stack_pointer_delta = 0;

    // If set, don't commit this instruction to architectural state upon
    // retirement.
    bool invalid;
    // Should be null in most cases, but if an interrupt is triggered, the vector to jump
    // to will be stored here (and for faults all instructions in the pipeline will be marked
    // invalid upon retirement).
    uint32_t exception_vector;
};

}

#endif // INC_CORE_CPU_H