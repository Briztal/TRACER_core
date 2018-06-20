/*
  arm_cortex_m4f.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CORE_ARM_CORTEXM4F
#define CORE_ARM_CORTEXM4F

#include "stdbool.h"

#include "stdint.h"

#include "stddef.h"

#include "kinetis.h"


/*
 * -------------------------------------- Types --------------------------------------
 */

//The process stack type;
typedef struct {
	
	//The current stack pointer;
    void *stack_pointer;

	//The initial of the stack;
    void *stack_begin;
	
	//The end of the stack;
    void *stack_end;

} core_stack_t;


//The empty stack initializer;
#define EMPTY_CORE_STACK() (core_stack_t) {.stack_pointer = 0, .stack_begin = 0, .stack_end = 0,}


//The core process structure;
typedef struct {

    //The process's stack;
    core_stack_t *process_stack;

    //The execution time granted to the process before its preemption;
    uint16_t activity_time;

} core_process_t;


//-------------------------------------- Init --------------------------------------

//Initialise the core;
void core_init();


//------------------------------------- Stack bounds -------------------------------------

//Correct a size to get a 4 bytes multiple;
inline size_t core_correct_size(size_t size);

//Get the stack begin from the allocated region;
inline void *core_get_stack_begin(void *allocated_pointer, size_t size);

//Get the stack end from the allocated region;
inline void *core_get_stack_end(void *allocated_pointer, size_t size);


//------------------------------------- Stack management  -------------------------------------

//Initialise a stack for a future unstacking. An arg pointer can be passed;
void core_init_stack(core_stack_t *stack, void (*function)(), void (*end_loop)(), void *init_arg);

//Get the argument stored at stack init. Must be called at the very initial of the process function;
void *core_get_init_arg();


//------------------------------------- Execution -------------------------------------

//Set the process provider;
void core_set_process_provider(void (*stack_provider)(core_process_t *));

//Reset the process provider;
void core_reset_process_provider();

//Execute a process, defined by its initialised stack, its duration, and its function. An arg can be passed;
void core_execute_process(core_process_t *process, void (*function)(void *), void *arg);

//Trigger the preemption;
inline void core_preempt_process();


//------------------------------------- Execution modes and privilleges -------------------------------------

//Returns true if the current code runs in thread mode
inline bool core_in_handler_mode();


//------------------------------------- Interrupts ------------------------------------- 

/*
 * Priority levels : these are the standard interrupt priorities for drivers;
 *
 * Cortex-M4: 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240
 */


//The standard priority for status interrupt;
#define DRIVER_STARUS_INTERRUPT_PRIORITY 32

//The standard priority for error interrupt;
#define DRIVER_ERROR_INTERRUPT_PRIORITY 16


//The default interrupt handler;
void CORE_IC_default_handler(void);

//Enable all interrupts;
inline void core_enable_interrupts();

//Disable all interrupts;
inline void core_disable_interrupts();


//Enable the specified interrupt;
inline void core_IC_enable(uint8_t int_channel);

//Disable the specified interrupt
inline void core_IC_disable(uint8_t int_channel);

//Mark the interrupt pending. Will be executed asap;
inline void core_IC_set_pending(uint8_t int_channel);
  
//Mark the interrupt as not pending;
inline void core_IC_clear_pending(uint8_t int_channel);

//Set the priority of the required interrupt to the povided;
inline void core_IC_set_priority(uint8_t int_channel, uint8_t priority);

//Set the handler of the required interrupt;
inline void core_IC_set_handler(uint8_t int_channel, void (*handler)());

//Set the default handler for the required interrupt;
inline void core_IC_set_default_handler(uint8_t int_channel);

//Set the handler of the required interrupt;
inline void *core_IC_get_handler_address(uint8_t int_channel);




/*
 * ------------------------------------- Stack bounds -------------------------------------
 */

/*
 * core_correct_size : corrects the size to get a multiple of 4.
 */

inline size_t core_correct_size(size_t size) {
	
	//Set the two first bytes to zero to obtain a multiple of 8;
	return size & ~(size_t)7;
	
}


/*
 * core_get_stack_begin : determines the stacks initial case from the allocated pointer and the size;
 *
 *  In an arm cortex, the stack decreases, and the stack pointer points to the last pushed element;
 */

inline void *core_get_stack_begin(void *allocated_pointer, size_t size) {

    //Count 16 bytes less to avoid stack underflow;
    return  (void *) ((uint32_t)(allocated_pointer) + ((uint32_t)size - 16));
}


/*
 * core_get_stack_end : determines the stacks end case from the allocated pointer and the size;
 *
 *  In an arm cortex, the stack decreases, and the stack pointer points to the last pushed element;
 */

inline void *core_get_stack_end(void *allocated_pointer, size_t unused) {
	return  allocated_pointer;
}



/*
 * ------------------------------------- Context switch -------------------------------------
 */

/*
 * core_preempt_process : sets the pendSV exception isPending flag;
 */

inline void core_preempt_process() {
    SCB_ICSR |= SCB_ICSR_PENDSVSET;
}


/*
 * ------------------------------------- Execution modes and privilleges -------------------------------------
 */

/*
 * core_in_handler_mode : this function returns true if the current code is ran from an interrupt;
 *
 *  If we are an interrupt, the 8 first bits of the System Control Block will contain the exception number.
 *
 *  If those are 0, no interupt is running yet;
 */

inline bool core_in_handler_mode() {
    return (bool) ((uint8_t) SCB_ICSR);
}


/*
 * ------------------------------------- Interrupts -------------------------------------
 */

/*
 * core_enable_interrupts : injects inline assembly code to disable all interrupts. 
 * 	Fault handling is still enabled;
 */

inline void core_enable_interrupts() {
	__asm__ __volatile__("cpsie i");
}


/*
 * core_disable_interrupts : injects inline assembly code to disable all interrupts. 
 * 	Fault handling is still enabled;
 */

inline void core_disable_interrupts() {
	__asm__ __volatile__("cpsid i");
}


/*
 * core_IC_enable : enables the specified interrupt;
 */

inline void core_IC_enable(const uint8_t int_channel) {
    NVIC_ENABLE_IRQ(int_channel);
}


/*
 * core_IC_disable : disables the specified interrupt;
 */

inline void core_IC_disable(const uint8_t int_channel) {
    NVIC_DISABLE_IRQ(int_channel);
}


/*
 * core_IC_set_pending : marks the interrupt as pending. Will be executed asap;
 */

inline void core_IC_set_pending(const uint8_t int_channel) {
    NVIC_SET_PENDING(int_channel);
}


/*
 * core_IC_clear_pending :  marks the interrupt as not pending;
 */

inline void core_IC_clear_pending(const uint8_t int_channel) {
    NVIC_CLEAR_PENDING(int_channel);
}


/*
 * core_IC_set_priority : sets the priority of the required interrupt to the povided;
 */

inline void core_IC_set_priority(const uint8_t int_channel, uint8_t priority) {
    NVIC_SET_PRIORITY(int_channel, priority);
}


/*
 * core_IC_set_handler : sets the handler of the required interrupt;
 */

inline void core_IC_set_handler(const uint8_t int_channel, void (*handler)()) {
    _VectorsRam[16 + int_channel] = handler;
}


/*
 * core_IC_set_default_handler : sets the default handler of the required interrupt;
 */

inline void core_IC_set_default_handler(const uint8_t int_channel) {

	//Set the default handler for the required channel;
	core_IC_set_handler(int_channel, &CORE_IC_default_handler);

}


/*
 * core_IC_get_handler_address : sets the handler of the required interrupt;
 */

inline void *core_IC_get_handler_address(const uint8_t int_channel) {

	//Return the appropriate address of the handler array;
	return _VectorsRam + 16 + int_channel;

}



#endif