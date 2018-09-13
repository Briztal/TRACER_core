/*
  timer.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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


#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

#include "stdbool.h"


//-------------------------------------------------- Timer interface ---------------------------------------------------

struct timer_interface {

	//Update the base frequency;
	void (*const set_base_frequency)(uint32_t base_frequency);


	//Start the timer;
	void (*const start)();

	//Stop the timer;
	void (*const stop)();

	//Is the timer started ?;
	bool (*const started)();


	//Set the timer's count;
	void (*const set_count)(uint32_t period);

	//Get the timer's count;
	uint32_t (*const get_count)();


	//Set the reload interrupt value, and adjust the count so that reload happens in this period;
	void (*const set_ovf_value)(uint32_t period);

	//Get the reload interrupt value;
	uint32_t (*const get_ovf_value)();


	//Enable the reload interrupt;
	void (*const enable_int)();

	//Enable the reload interrupt;
	void (*const disable_int)();

	//Is the reload interrupt enabled ?
	bool (*const int_enabled)();


	//Is the interrupt flag enabled ?
	bool (*const flag_is_set)();

	//Is the interrupt flag enabled ?
	void (*const flag_clr)();


	//Update the interrupt handler;
	void (*const set_handler)(void (*handler)());

};


//The timer lib includes a neutral timer interface, so that timer drivers can prevent access to their functions;
extern const struct timer_interface neutral_timer_interface;


//---------------------------------------------------- Timer config ----------------------------------------------------

#define t_if timer_interface

//Configures the timer and starts it.
void timer_init(const struct t_if *, uint32_t base_frequency, uint32_t ovf_period, void (*handler)());

//Stop the timer, stop the interrupt, reset counts, reset the handler to 0;
void timer_reset(const struct t_if *, uint32_t base_frequency);


//------------------------------------------------ Inline function exec ------------------------------------------------


//A macro to generate a static inline for a timer member function that takes no special args
#define t_if_inline(name, ret_type)\
static inline ret_type timer_##name(const struct t_if *timer) {\
	return (*(timer->name))();\
}

//A macro to generate a static inline for a timer member function that takes one argument;
#define t_if_inline_a(name, arg_type)\
static inline void timer_##name(const struct t_if *timer, arg_type __timer_arg__) {\
	(*(timer->name))(__timer_arg__);\
}



/*
 * All timer interface fields have their static inline shortcuts. To access a member x, use timer_x with appropriate
 * 	arguments;
 */

t_if_inline_a(set_base_frequency, uint32_t)
t_if_inline(start, void)
t_if_inline(stop, void)
t_if_inline(started, bool)
t_if_inline_a(set_count, uint32_t)
t_if_inline(get_count, uint32_t)
t_if_inline_a(set_ovf_value, uint32_t)
t_if_inline(get_ovf_value, uint32_t)
t_if_inline(enable_int, void)
t_if_inline(disable_int, void)
t_if_inline(int_enabled, bool)
t_if_inline(flag_is_set, bool)
t_if_inline(flag_clr, void)


//Set the timer count value;
static inline void timer_set_handler(const struct t_if *timer, void (*handler)()) {
	(*(timer->set_handler))(handler);
}

#endif
