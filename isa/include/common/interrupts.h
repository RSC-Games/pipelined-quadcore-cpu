#ifndef INC_CORE_INTERRUPTS_H
#define INC_CORE_INTERRUPTS_H

#include <cstdint>

namespace isa {
// CPU Exception Vectors.
const uint32_t INT_LOAD_FAULT = 0;  // Memory Fault
const uint32_t INT_STORE_FAULT = 4;  // Memory Fault
const uint32_t INT_PAGE_FAULT = 8;  // MMU Fault
const uint32_t INT_INVALID_PAGE_FAULT = 12;  // MMU Fault.
const uint32_t INT_LOAD_STORE_ALIGNMENT_FAULT = 16;  // Memory Fault.
const uint32_t INT_INVALID_OPCODE_FAULT = 20;  // CPU Fault.
const uint32_t INT_INSTRUCTION_FETCH_FAULT = 24;  // Memory Fault.
const uint32_t INT_INSTR_FETCH_ALIGNMENT_FAULT = 28;  // Memory Fault.
const uint32_t INT_ZERO_DIVISION_FAULT = 32;  // CPU Fault.
const uint32_t INT_DOUBLE_FAULT = 36;  // CPU Fault.

// CPU Interrupt Vectors.
const uint32_t INT_WDT_EXPIRED = 40;
const uint32_t INT_IRQL0 = 44;
const uint32_t INT_IRQL1 = 48;
const uint32_t INT_IRQL2 = 52;
const uint32_t INT_IRQL3 = 56;
const uint32_t INT_OS_TIMER_TRIGGER = 60;
const uint32_t TIM0_COMP = 64;
const uint32_t TIM1_COMP = 68;
const uint32_t TIM2_COMP = 72;
const uint32_t TIM3_COMP = 76;
}

#endif  // INC_CORE_INTERRUPTS_H