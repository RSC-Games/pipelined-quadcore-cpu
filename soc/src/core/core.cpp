#include "core/core.h"
#include "core/memory.h"
#include "core/mmu.h"
#include "soc.h"

#include "common/interrupts.h"
#include "common/isa_decode.h"

#include "util/logger.h"
#include "config.h"

#include <iostream>
#include <cstdlib>

// TODO: Make shim library that hides this.
#if __MINGW32__
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
#endif

namespace core {

Core::Core(core::Memory* mem, core::BootROM* bootrom) {
    this->register_file = new unsigned int[16] {0};
    this->extended_registers = new unsigned int[16] {0};
    this->pc = 0xFFFFFFFC;  // Core boots 4 bytes before the end of addressable memory.
    this->priv_lvl = core::PRIV_0_KERNEL_MODE; // Core starts in kernel mode.
    this->mmu = new core::MMU(mem, bootrom);

    instr_table = new instr_table_ptr[256] {
        nullptr,
        &op_add,
        &op_sub,
        &op_mul,
        &op_div,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &op_and,
        &op_or,
        &op_xor,
        &op_not,
        &op_mov,
        &op_mov_imm,
        &op_mov_imm_high,
        &op_mov_ext,
        &op_mov_gp,
        &op_mov_ext2,
        &op_mov_ext_imm,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &op_ldb,
        &op_stb,
        &op_ld,
        &op_st,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &op_lsh,
        &op_lsh_imm,
        &op_rsh,
        &op_rsh_imm,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &op_jmp_imm,
        &op_jmp,
        &op_jmp_abs,
        &op_jmp_rel,
        &op_bnx,
        &op_cmp,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &op_call_rel,
        &op_call_imm,
        &op_call,
        &op_ret,
        &op_push,
        &op_pop,
        &op_push_ext,
        &op_pop_ext,
        &op_syscall,
        &op_sysret,
        &op_hlt,
        &op_cpuid
    };

    // Initialize pipeline.
    this->pipeline_stages = new PipelineStage*[5];

    for (int i = 0; i < 5; i++) {
        this->pipeline_stages[i] = new PipelineStage {
            this->pc,               // PC at instruction issue
            0x29000000,             // Fully encoded instruction (stage 1)
            nullptr,                // Decoded instruction data (stage 2)
            &op_nop,                // Stage 3 payload (execute)
            &stage4_nop,            // Stage 4 payload (mem io)
            0,                      // mem_addr (stage 4)
            0,                      // mem_data (stage 4)
            &stage5_commit_invalid, // Stage 5 payload (commit)
            0,                      // register id (stage 5)
            0,                      // register value (stage 5)
            0,                      // stack pointer delta (usually 0, stage 5)
            true,                   // filler instruction is not valid (checked at retirement)
            0                       // active fault exception vector (null means no exception)
        };
    }

    // Avoid leaking memory in the first two pipeline stages
    for (int i = 2; i < 5; i++) {
        this->pipeline_stages[i]->decoded_instr = new isa::DecodedInstruction {
            new uint32_t[] {0, 0},
            2,
            0x29
        };
    }

    // Stage 1 fetch is the only valid stage at bootup (since it hasn't done anything)
    this->pipeline_stages[0]->invalid = false;
}

Core::~Core() {
    this->halted = true;
    this->priv_lvl = core::PRIV_1_USER_MODE;
    this->pc = 0x0;
    delete[] this->register_file;
    delete[] this->extended_registers;
    delete[] this->pipeline_stages;
    delete[] this->instr_table;
}

void Core::CORE_set_halt_state(bool halt) {
    this->halted = halt;
}

void Core::CORE_set_clock_rate(uint32_t clock_rate_MHz) {
    this->clock_rate = 1000000 * clock_rate_MHz;
}

void Core::simulate() {
    // TODO: Clock simulation. Currently just runs at maximum possible
    // speed.
    while (true) {
        // try {
            this->CORE_run_pipeline();
        
        //     // TODO: make portable.
        //     LOG_INFO("Executed thread... waiting for next instruction.");
        //     // TODO: Make sleep library... maybe?
            Sleep(1000);
        // }
        // // TODO: Catch emulated CPU exceptions. Currently unsupported.
        // catch (uint32_t exc) {
        //     this->print_exception_frame(exc);

        //     // TODO: Exceptions cause a jump to a handler. Do this.
        // }
    }
}

// Only intended for core debugging (VM should be dumping its own state
// with emulated code).
void Core::print_exception_frame(uint32_t exception) {
    LOG_ERROR("Received uncaught CPU exception!");

    // TODO: Change to a lookup table?
    std::string exception_name = "UNDEFINED";

    switch (exception) {
        case isa::INT_LOAD_FAULT:
            exception_name = "LoadFault";
            break;
        case isa::INT_STORE_FAULT:
            exception_name = "StoreFault";
            break;
        case isa::INT_PAGE_FAULT:
            exception_name = "PageFault";
            break;
        case isa::INT_INVALID_PAGE_FAULT:
            exception_name = "InvalidPageFault";
            break;
        case isa::INT_LOAD_STORE_ALIGNMENT_FAULT:
            exception_name = "LoadStoreAlignmentFault";
            break;
        case isa::INT_INVALID_OPCODE_FAULT:
            exception_name = "InvalidOpcodeFault";
            break;
        case isa::INT_INSTRUCTION_FETCH_FAULT:
            exception_name = "InstructionFetchFault";
            break;
        case isa::INT_INSTR_FETCH_ALIGNMENT_FAULT:
            exception_name = "ZeroDivisionFault";
            break;
        case isa::INT_DOUBLE_FAULT:
            exception_name = "DoubleFault";
            break;
    }

    std::cout << "Fatal: " << exception_name << " encountered at PC 0x" << std::hex << this->pc << "\n";
    std::cout << "Execution trapped. Hit enter to continue. \n> ";

    std::string _ = "";
    std::getline(std::cin, _);
}

void Core::CORE_instr_fetch() {
    LOG_INFO("============================ PIPELINE STAGE 1 ============================");
    PipelineStage* current = this->pipeline_stages[0];
    current->pc_val = this->pc;

    uint32_t phy_addr = this->mmu->tlb_translate(this->pc);
    uint32_t instruction = this->mmu->load_word(phy_addr);

    printf("fetched instruction opcode 0x%X from addr 0x%X\n", instruction, current->pc_val);
    current->instruction_bytes = instruction;

    // TODO: Branch target prediction (start fetching at the predicted target if the branch
    // is predicted taken). If no target prediction exists and the branch is predicted not
    // taken, fetch instructions past the branch. If the branch is predicted taken and no
    // target exists, straight line speculate (and update the target buffer once the branch
    // is retired).
    // For jumps (which are guaranteed taken), only predict the target, and if the target
    // cannot be predicted, straight line speculate.
}

void Core::CORE_decode_instr() {
    LOG_INFO("============================ PIPELINE STAGE 2 ============================");
    PipelineStage* current = this->pipeline_stages[1];

    try {
        isa::DecodedInstruction* decoded = isa::decode_instr(current->instruction_bytes);
        current->decoded_instr = decoded;

        current->instr_func = !current->invalid ? resolve_instr_table_ptr(current->decoded_instr->instruction)
            : &op_nop;
    }
    catch (uint32_t exception) {
        LOG_WARNING("cpu fault during decode; marking as erroneous");
        current->decoded_instr = isa::get_empty_instr();
        // When retired, will need to mark all previous instructions invalid.
        current->exception_vector = 0xDEADBEEF; // NOT IMPLEMENTED
        current->instr_func = &op_nop; // Don't wanna do anything.
        current->stage4 = &stage4_nop;
        current->stage5 = &stage5_commit_invalid;
    }
}

void Core::CORE_execute_op() {
    LOG_INFO("============================ PIPELINE STAGE 3 ============================");
    PipelineStage* current = this->pipeline_stages[2];

    isa::DecodedInstruction* decoded = current->decoded_instr;
    printf("*** PREPARING TO EXEC: instr 0x%X operand cnt %d\n", decoded->instruction, decoded->operand_cnt);

    // List of instruction operands
    for (int i = 0; i < decoded->operand_cnt; i++)
        printf(" op%d: 0x%X", i, decoded->operands[i]);

    printf("\n");

    // Execute stage3
    (this->*(current->instr_func))(*current);
}

void Core::CORE_mem_access() {
    LOG_INFO("============================ PIPELINE STAGE 4 ============================");
    PipelineStage* current = this->pipeline_stages[3];

    // Execute stage4 payload.
    (this->*(current->stage4))(*current);
}

void Core::CORE_commit_last_instr() {
    LOG_INFO("============================ PIPELINE STAGE 5 ============================");
    // TODO: Write back the cached ALU output to a register (if necessary), then
    // retire the instruction.
    PipelineStage* current = this->pipeline_stages[4];
    printf("retiring instruction: reg id %d reg data 0x%X invalid? %d\n", current->reg_id, current->reg_commit_val, current->invalid);

    // TODO: determine if instruction should be retired or if the entire pipeline should be invalidated.
    // Stage 4 doesn't matter for invalid instructions.
    // TODO: Will be removed.
    if (current->invalid && current->stage5 != &stage5_commit_invalid) {
        LOG_ERROR("instruction marked as invalid but has pipeline side effects!");

        //abort();
    }

    // Execute stage5 payload (if we're retiring it)
    (this->*(current->stage5))(*current);

    // Exception was triggered; pipeline flush will be necessary soon (ish).
    // TODO: perform PC jump.
    if (!current->invalid && current->exception_vector != 0) {
        LOG_ERROR("INSTRUCTION RETIRED WITH EXCEPTION STATUS!!!!! not implemented");
        throw current->exception_vector;
    }
}

void Core::CORE_run_pipeline() {
    // TODO: will require hazard detection (commit stage/execute require the same registers, data
    //  dependency between 2 instructions)
    this->CORE_instr_fetch();
    this->CORE_decode_instr();
    this->CORE_execute_op();
    this->CORE_mem_access();
    this->CORE_commit_last_instr();

    // DEBUGGING: Print current pipeline state.
    #if VERBOSE_CPU
        LOG_INFO("######################## Current pipeline state ########################");
        printf("stg1/fetch i:%i/e:%i: instr 0x%X prefetch PC 0x%08X \n", this->pipeline_stages[0]->invalid, 
            this->pipeline_stages[0]->exception_vector != 0, this->pipeline_stages[0]->instruction_bytes, this->pc - 0x4);
        printf("stg2/decode i:%i/e:%i: instr 0x%X decoded opcode 0x%X\n", this->pipeline_stages[1]->invalid, 
            this->pipeline_stages[1]->exception_vector != 0, this->pipeline_stages[1]->instruction_bytes, 
            this->pipeline_stages[1]->decoded_instr->instruction);
        printf("stg3/exec i:%i/e:%i: instr 0x%X op cnt %d\n", this->pipeline_stages[2]->invalid, 
            this->pipeline_stages[2]->exception_vector != 0, this->pipeline_stages[2]->instruction_bytes, 
            this->pipeline_stages[2]->decoded_instr->operand_cnt);
        printf("stg4/memop i:%i/e:%i: instr 0x%X stg4 0x%p addr 0x%X data 0x%X\n", this->pipeline_stages[3]->invalid, 
            this->pipeline_stages[3]->exception_vector != 0, this->pipeline_stages[3]->instruction_bytes, this->pipeline_stages[3]->stage4, 
            this->pipeline_stages[3]->mem_addr, this->pipeline_stages[3]->mem_data);
        printf("stg5/commit i:%i/e:%i: instr 0x%X stg5 0x%p reg_id %d val 0x%X\n", this->pipeline_stages[4]->invalid, 
            this->pipeline_stages[4]->exception_vector != 0, this->pipeline_stages[4]->instruction_bytes, this->pipeline_stages[4]->stage5, 
            this->pipeline_stages[4]->reg_id, this->pipeline_stages[4]->reg_commit_val);
        LOG_INFO("######################## Current pipeline state ########################");
        std::cout << "\n";
        
        LOG_INFO("######################## CPU REGISTER FILE DUMP ########################");
        printf("Core (X): (No active fault condition)\n");
        printf("X0:  0x%08X   X1:  0x%08X   X2:  0x%08X    X3:  0x%08X\n", this->register_file[REG_X0], this->register_file[REG_X1], 
            this->register_file[REG_X2], this->register_file[REG_X3]);
        printf("X4:  0x%08X   X5:  0x%08X   X6:  0x%08X    X7:  0x%08X\n", this->register_file[REG_X4], this->register_file[REG_X5], 
            this->register_file[REG_X6], this->register_file[REG_X7]);
        printf("X8:  0x%08X   X9:  0x%08X   X10: 0x%08X    X11: 0x%08X\n", this->register_file[REG_X8], this->register_file[REG_X9], 
            this->register_file[REG_X10], this->register_file[REG_X11]);
        printf("X12: 0x%08X   XRV: 0x%08X   XFP: 0x%08X    XSP: 0x%08X\n", this->register_file[REG_X12], this->register_file[REG_XRV], 
            this->register_file[REG_XFP], this->register_file[REG_XSP]);
        printf("E0:  0x%08X   E1:  0x%08X   E2:  0x%08X    E3:  0x%08X\n", this->extended_registers[REG_X0], this->extended_registers[REG_X1], 
            this->extended_registers[REG_X2], this->extended_registers[REG_X3]);
        printf("E4:  0x%08X   E5:  0x%08X   E6:  0x%08X    E7:  0x%08X\n", this->extended_registers[REG_X4], this->extended_registers[REG_X5], 
            this->extended_registers[REG_X6], this->extended_registers[REG_X7]);
        printf("E8:  0x%08X   E9:  0x%08X   E10: 0x%08X    E11: 0x%08X\n", this->extended_registers[REG_X8], this->extended_registers[REG_X9], 
            this->extended_registers[REG_X10], this->extended_registers[REG_X11]);
        printf("E12: 0x%08X   E13: 0x%08X   E14: 0x%08X    E15: 0x%08X\n", this->extended_registers[REG_X12], this->extended_registers[REG_XRV], 
            this->extended_registers[REG_XFP], this->extended_registers[REG_XSP]);
        printf("PC:  0x%08X (prefetch)\n", this->pc); // Use architectural PC or prefetch PC? (using prefetch)
        LOG_INFO("######################## CPU REGISTER FILE DUMP ########################");
        std::cout << "\n\n";
    #endif

    // Rotate pipeline entries (and simulate instructions flowing through the pipeline) and retire the last one.
    PipelineStage* retired = this->pipeline_stages[4];

    // Catch instructions not decoding right (buggy implementation)
    // Probably not necessary to ship
    retired->stage5 = nullptr;
    retired->stage4 = nullptr;
    retired->instr_func = nullptr;
    retired->mem_addr = 0xCEAECEAE;
    retired->mem_data = 0xFEAEFEAE;
    retired->reg_id = -1;
    retired->reg_commit_val = 0xCAFECAFE;
    retired->pc_val = 0xDEADDEAD;

    retired->stack_pointer_delta = 0;
    retired->exception_vector = 0;
    retired->invalid = false;

    // Get rid of the stale decoded instruction.
    delete[] retired->decoded_instr->operands;
    delete retired->decoded_instr;
    retired->decoded_instr = nullptr;

    for (int i = 4; i > 0; i--)
        this->pipeline_stages[i] = this->pipeline_stages[i-1];

    this->pipeline_stages[0] = retired;
}

inline Core::instr_table_ptr Core::resolve_instr_table_ptr(short instr_id) {
    return this->instr_table[instr_id];
}

#pragma region Stage3ExecutableOpcodes

void Core::op_nop(PipelineStage& instr) {
    // TODO: Add new stage5 for exception PC commit?
    instr.stage4 = &stage4_nop;
    instr.stage5 = &stage5_commit_invalid;
}

void Core::op_add(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int src_reg = instr.decoded_instr->operands[1];
    
    // Add doesn't perform memory operations but commits a single general purpose register
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] + this->register_file[src_reg];
    instr.reg_id = dest_reg;
}

void Core::op_sub(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int src_reg = instr.decoded_instr->operands[1];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] + ~this->register_file[src_reg] + 1;
    instr.reg_id = dest_reg;
}

void Core::op_mul(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int src_reg = instr.decoded_instr->operands[1];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] * this->register_file[src_reg];
    instr.reg_id = dest_reg;
}

void Core::op_div(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int src_reg = instr.decoded_instr->operands[1];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] / this->register_file[src_reg];
    instr.reg_id = dest_reg;
}

void Core::op_and(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int src_reg = instr.decoded_instr->operands[1];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] & this->register_file[src_reg];
    instr.reg_id = dest_reg;
}

void Core::op_or(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int src_reg = instr.decoded_instr->operands[1];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] | this->register_file[src_reg];
    instr.reg_id = dest_reg;
}

void Core::op_xor(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int src_reg = instr.decoded_instr->operands[1];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] ^ this->register_file[src_reg];
    instr.reg_id = dest_reg;
}

void Core::op_not(PipelineStage& instr) { 
    int operand_reg = instr.decoded_instr->operands[0];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = ~this->register_file[operand_reg];
    instr.reg_id = operand_reg;
}

void Core::op_mov(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int src_reg = instr.decoded_instr->operands[1];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[src_reg];
    instr.reg_id = dest_reg;
}

void Core::op_mov_imm(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = instr.decoded_instr->operands[1];
    instr.reg_id = dest_reg;
}

void Core::op_mov_imm_high(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = instr.decoded_instr->operands[1] << 16;
    instr.reg_id = dest_reg;
}

void Core::op_mov_ext(PipelineStage& instr) {
    int dest_extreg = instr.decoded_instr->operands[0];
    int src_gpreg = instr.decoded_instr->operands[1];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_extreg;
    instr.reg_commit_val = this->register_file[src_gpreg];
    instr.reg_id = dest_extreg;
}

void Core::op_mov_gp(PipelineStage& instr) {
    int dest_gpreg = instr.decoded_instr->operands[0];
    int src_extreg = instr.decoded_instr->operands[1];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->extended_registers[src_extreg];
    instr.reg_id = dest_gpreg;
}

void Core::op_mov_ext2(PipelineStage& instr) {
    int dest_extreg = instr.decoded_instr->operands[0];
    int src_extreg = instr.decoded_instr->operands[1];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_extreg;
    instr.reg_commit_val = this->extended_registers[src_extreg];
    instr.reg_id = dest_extreg;
}

void Core::op_mov_ext_imm(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_extreg;
    instr.reg_commit_val = instr.decoded_instr->operands[1];
    instr.reg_id = dest_reg;
}

void Core::op_ldb(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int addr_reg = instr.decoded_instr->operands[1];
    int offset_reg = instr.decoded_instr->operands[2];

    instr.stage4 = &stage4_load_byte;
    instr.mem_addr = this->register_file[addr_reg] + this->register_file[offset_reg];
    // mem_data unused for data load (loaded data is stored in reg_commit_val)
    instr.stage5 = &stage5_commit_gpreg;
    // reg_commit_val filled by stage4.
    instr.reg_id = dest_reg;
}

void Core::op_stb(PipelineStage& instr) {
    int src_reg = instr.decoded_instr->operands[0];
    int addr_reg = instr.decoded_instr->operands[1];
    int offset_reg = instr.decoded_instr->operands[2];

    instr.stage4 = &stage4_store_byte;
    instr.mem_addr = this->register_file[addr_reg] + this->register_file[offset_reg];
    instr.mem_data = this->register_file[src_reg];
    instr.stage5 = &stage5_commit_null;
    // Stage 5 unused by stores.
}

void Core::op_ld(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int addr_reg = instr.decoded_instr->operands[1];
    int offset_reg = instr.decoded_instr->operands[2];
    
    instr.stage4 = &stage4_load_word;
    instr.mem_addr = this->register_file[addr_reg] + this->register_file[offset_reg];
    // mem_data unused for data load (loaded data is stored in reg_commit_val)
    instr.stage5 = &stage5_commit_gpreg;
    // reg_commit_val filled by stage4.
    instr.reg_id = dest_reg;
}

void Core::op_st(PipelineStage& instr) {
    int src_reg = instr.decoded_instr->operands[0];
    int addr_reg = instr.decoded_instr->operands[1];
    int offset_reg = instr.decoded_instr->operands[2];
    
    instr.stage4 = &stage4_store_word;
    instr.mem_addr = this->register_file[addr_reg] + this->register_file[offset_reg];
    instr.mem_data = this->register_file[src_reg];
    instr.stage5 = &stage5_commit_null;
    // Stage 5 unused by stores.
}

void Core::op_lsh(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int shift_reg = instr.decoded_instr->operands[1];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] << this->register_file[shift_reg];
    instr.reg_id = dest_reg;
}

void Core::op_lsh_imm(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] << instr.decoded_instr->operands[1];
    instr.reg_id = dest_reg;
}

void Core::op_rsh(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    int shift_reg = instr.decoded_instr->operands[1];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] >> this->register_file[shift_reg];
    instr.reg_id = dest_reg;
}

void Core::op_rsh_imm(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];
    
    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = this->register_file[dest_reg] >> instr.decoded_instr->operands[1];
    instr.reg_id = dest_reg;
}

void Core::op_jmp_imm(PipelineStage& instr) {
    int addr_reg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_pc;
    instr.reg_commit_val = this->register_file[addr_reg] + instr.decoded_instr->operands[1];
    // reg_id not needed for a PC commit
}

void Core::op_jmp(PipelineStage& instr) {
    int addr_reg = instr.decoded_instr->operands[0];
    int offset_reg = instr.decoded_instr->operands[1];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_pc;
    instr.reg_commit_val = this->register_file[addr_reg] + this->register_file[offset_reg];
    // reg_id not needed for a PC commit
}

void Core::op_jmp_abs(PipelineStage& instr) {
    int addr_reg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_pc;
    instr.reg_commit_val = this->register_file[addr_reg];
    // reg_id not needed for a PC commit
}

void Core::op_jmp_rel(PipelineStage& instr) {
    // Sign extend the pc offset to 32 bits (from 20) to permit forward and backward jumps
    int jmp_rel_pc_offset = ((int)instr.decoded_instr->operands[0] << 12) >> 12;
    printf("got jump offset 0x%X, PC=0x%X, PC+jmp=0x%X\n", jmp_rel_pc_offset, instr.pc_val, instr.pc_val + jmp_rel_pc_offset);

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_pc;
    // Already in two's complement; no reason to do that here.
    instr.reg_commit_val = instr.pc_val + jmp_rel_pc_offset;
    // reg_id not needed for a PC commit
}

void Core::op_bnx(PipelineStage& instr) {
    bool branch_if_zero = static_cast<boolean>(instr.decoded_instr->operands[0]);
    int operand_reg = instr.decoded_instr->operands[1];
    int addr_reg = instr.decoded_instr->operands[2];
    int offset_reg = instr.decoded_instr->operands[3];

    bool operand_is_zero = !static_cast<boolean>(register_file[operand_reg]);

    if (operand_is_zero == branch_if_zero) {
        instr.stage5 = &stage5_commit_pc;
        instr.reg_commit_val = this->register_file[addr_reg] + this->register_file[offset_reg];
        // reg_id not needed for a PC commit
    }
    else
        // Prevent pipeline invalidation at retirement.
        instr.stage5 = &stage5_commit_null;

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
}

void Core::op_cmp(PipelineStage& instr) {
    int comparison_code = instr.decoded_instr->operands[0];
    int dest_reg = instr.decoded_instr->operands[1];
    int operand_1 = this->register_file[instr.decoded_instr->operands[2]];
    int operand_2 = this->register_file[instr.decoded_instr->operands[3]];

    unsigned int commit;

    switch (comparison_code) {
        case CMP_COND_EQUIVALENT:
            commit = operand_1 == operand_2;
            break;
        case CMP_COND_LESS_THAN:
            commit = operand_1 < operand_2;
            break;
        case CMP_COND_GREATHER_THAN:
            commit = operand_1 > operand_2;
            break;
        case CMP_COND_LESS_OR_EQUAL:
            commit = operand_1 <= operand_2;
            break;
        case CMP_COND_GREATER_OR_EQUAL:
            commit = operand_1 >= operand_2;
            break;
        case CMP_COND_NOT_EQUAL:
            commit = operand_1 != operand_2;
            break;
    }

    instr.stage4 = &stage4_nop;
    // Don't set stage 4 parameters (not in use)
    instr.stage5 = &stage5_commit_gpreg;
    instr.reg_commit_val = commit;
    instr.reg_id = dest_reg;
}

void Core::op_call_rel(PipelineStage& instr) {
    // Sign extend the pc offset to permit forward and backward jumps
    int rel_pc_offset = ((int)instr.decoded_instr->operands[0] << 16) >> 16;
    
    // Offset is already in two's complement; no reason to recompute that...

    instr.stage4 = &stage4_store_word;
    instr.mem_addr = this->register_file[REG_XSP] - 0x4; // SP points at last word and stack grows down.
    instr.mem_data = instr.pc_val;
    instr.stage5 = &stage5_commit_pc;
    instr.reg_commit_val = instr.pc_val + rel_pc_offset;
    instr.stack_pointer_delta = 0x4;
    // reg_id not needed for a PC commit
}

void Core::op_call_imm(PipelineStage& instr) {
    int addr_reg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_store_word;
    instr.mem_addr = this->register_file[REG_XSP] - 0x4;
    instr.mem_data = instr.pc_val;
    instr.stage5 = &stage5_commit_pc;
    instr.reg_commit_val = this->register_file[addr_reg] + instr.decoded_instr->operands[1];
    instr.stack_pointer_delta = 0x4;
    // reg_id not needed for a PC commit
}

void Core::op_call(PipelineStage& instr) {
    int addr_reg = instr.decoded_instr->operands[0];
    int offset_reg = instr.decoded_instr->operands[1];

    instr.stage4 = &stage4_store_word;
    instr.mem_addr = this->register_file[REG_XSP] - 0x4;
    instr.mem_data = instr.pc_val;
    instr.stage5 = &stage5_commit_pc;
    instr.reg_commit_val = this->register_file[addr_reg] + this->register_file[offset_reg];
    instr.stack_pointer_delta = 0x4;
    // reg_id not needed for a PC commit
}

void Core::op_ret(PipelineStage& instr) {
    instr.stage4 = &stage4_load_word;
    instr.mem_addr = this->register_file[REG_XSP];
    // mem_data not used for loads.
    instr.stage5 = &stage5_commit_pc;
    // reg_commit_val set by stage4 payload
    // reg_id not required for PC commit.
    instr.stack_pointer_delta = -0x4;
}

void Core::op_push(PipelineStage& instr) {
    int src_reg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_store_word;
    instr.mem_addr = this->register_file[REG_XSP] - 0x4;
    instr.mem_data = this->register_file[src_reg];
    instr.stage5 = &stage5_commit_null;
    // Stage 5 inactive (only changes stack pointer)
    instr.stack_pointer_delta = 0x4;
}

void Core::op_pop(PipelineStage& instr) {
    int dest_reg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_load_word;
    instr.mem_addr = this->register_file[REG_XSP];
    // mem_data not used for load
    instr.stage5 = &stage5_commit_gpreg;
    // reg_commit_val set by stage4.
    instr.reg_id = dest_reg;
    instr.stack_pointer_delta = -0x4;
}

void Core::op_push_ext(PipelineStage& instr) {
    int src_extreg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_store_word;
    instr.mem_addr = this->register_file[REG_XSP] - 0x4;
    instr.mem_data = this->extended_registers[src_extreg];
    instr.stage5 = &stage5_commit_null;
    // Stage 5 inactive (only changes stack pointer)
    instr.stack_pointer_delta = 0x4;
}

void Core::op_pop_ext(PipelineStage& instr) {
    int dest_extreg = instr.decoded_instr->operands[0];

    instr.stage4 = &stage4_load_word;
    instr.mem_addr = this->register_file[REG_XSP];
    // mem_data not used for load
    instr.stage5 = &stage5_commit_extreg;
    // reg_commit_val set by stage4.
    instr.reg_id = dest_extreg;
    instr.stack_pointer_delta = -0x4;
}

void Core::op_syscall(PipelineStage& instr) {
    // ID is a fixed offset into a syscall jump table (controlled by two
    // hardware config registers OS_TABLE_BASE and OS_TABLE_LENGTH)
    int id_reg = instr.decoded_instr->operands[0];

    // Save exception status for instruction retirement.
    throw isa::INT_INVALID_OPCODE_FAULT;
}

void Core::op_sysret(PipelineStage& /* instr */) {
    // Only executable in kernel mode (userland forbidden access)
    if (priv_lvl != PRIV_0_KERNEL_MODE)
        throw isa::INT_INVALID_OPCODE_FAULT;

    // Not implemented
    throw isa::INT_INVALID_OPCODE_FAULT;
}

void Core::op_hlt(PipelineStage& /* instr */) {
    // Only executable in kernel mode (userland forbidden access)
    if (priv_lvl != PRIV_0_KERNEL_MODE)
        throw isa::INT_INVALID_OPCODE_FAULT;

    // Not implemented
    throw isa::INT_INVALID_OPCODE_FAULT;
}

void Core::op_cpuid(PipelineStage& instr) {
    // Not implemented
    throw isa::INT_INVALID_OPCODE_FAULT;
}

#pragma endregion

// Pipeline stage 4 payloads (memory read/write)
#pragma region Stage4MemoryOperations

void Core::stage4_nop(PipelineStage& /* instr */) {
    // Nothing to do here (intentionally empty)
    LOG_INFO("GOT TO STAGE 4 (nop)");
}

void Core::stage4_load_byte(PipelineStage& instr) {
    printf("preparing memfetch to vaddr 0x%X\n", instr.mem_addr);
    instr.reg_commit_val = this->mmu->load_byte(instr.mem_addr);
}

void Core::stage4_store_byte(PipelineStage& instr) {
    printf("preparing memwrite to vaddr 0x%X data 0x%X\n", instr.mem_addr, instr.mem_data);
    this->mmu->store_byte(instr.mem_addr, instr.mem_data);
}

void Core::stage4_load_word(PipelineStage& instr) {
    printf("preparing memfetch to vaddr 0x%X\n", instr.mem_addr);
    instr.reg_commit_val = this->mmu->load_word(instr.mem_addr);
}

void Core::stage4_store_word(PipelineStage& instr) {
    printf("preparing memwrite to vaddr 0x%X data 0x%X\n", instr.mem_addr, instr.mem_data);
    this->mmu->store_word(instr.mem_addr, instr.mem_data);
}

#pragma endregion

// Pipeline stage 5 payloads (architectural commit)
#pragma region Stage5CommitPayloads

void Core::stage5_commit_invalid(PipelineStage& instr) {
    printf("stage5 commit invalid; only incrementing prefetch pc; ignoring xsp\n");
    this->pc += 0x4;
}

void Core::stage5_commit_null(PipelineStage& instr) {
    printf("stage5 commit null; incrementing PC\n");
    this->register_file[REG_XSP] -= instr.stack_pointer_delta;
    this->pc += 0x4;
}

void Core::stage5_commit_gpreg(PipelineStage& instr) {
    printf("stage5 commit gpreg id %d val 0x%X\n", instr.reg_id, instr.reg_commit_val);
    this->register_file[instr.reg_id] = instr.reg_commit_val;
    this->register_file[REG_XSP] -= instr.stack_pointer_delta;
    this->pc += 0x4;
}

void Core::stage5_commit_extreg(PipelineStage& instr) {
    printf("stage5 commit extreg id %d val 0x%X\n", instr.reg_id, instr.reg_commit_val);
    this->extended_registers[instr.reg_id] = instr.reg_commit_val;
    this->register_file[REG_XSP] -= instr.stack_pointer_delta;
    this->pc += 0x4;
}

void Core::stage5_commit_pc(PipelineStage& instr) {
    printf("stage5 commit pc val 0x%X\n", instr.reg_commit_val);
    this->register_file[REG_XSP] -= instr.stack_pointer_delta;
    this->pc = instr.reg_commit_val;

    // TODO: mark the entire pipeline as invalid except for this
    // retiring instruction (since PC jumps will guaranteed make
    // previous instructions irrelevant)
    for (int i = 0; i < 4; i++) {
        this->pipeline_stages[i]->invalid = true;
        this->pipeline_stages[i]->stage5 = &stage5_commit_invalid;
    }
}

#pragma endregion

}