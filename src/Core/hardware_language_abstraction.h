/*
  hardware_language_abstraction.h - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef HDWLABSTRACTION_H
#define HDWLABSTRACTION_H

/*
 * The Board Abstraction file.
 *
 * This file is made for : Arduino Mega - atmega2560.
 *
 * Here are defined all function related to the hardware (serial, I2C, delay etc...)
 *
 * Those will depend on the processor and the toolchain you use
 *
 * If you must implement functions over the base hardware functions, use a function.
 *
 * If you only must alias a function, use a macro, it's faster.
 */

//---------------------------------------------------Includes-----------------------------------------------------------

/*
 * Include here all libraries related to the board itself
 */

#include "Arduino.h"
#include <setjmp.h>


//----------------------------------------------------Delay-------------------------------------------------------------

//Uncomment this line to provide delay support across LowLevel
#define HL_DELAY


#ifdef HL_DELAY
/* The function to call to wait for a specified number of milliseconds
 *  void delay_ms(unsigned int time_ms);
 *//*
inline static void delay_ms(unsigned int time_ms){

}*/
#define delay_ms(ms) delay(ms)


/* The function to call wait for a specified number of microseconds
 * void delay_us(unsigned int time_us);
 */
/*inline static void delay_us(unsigned int time_us){

}*/
#define delay_us(us) delayMicroseconds(us)

#endif

//--------------------------------------StepperTimer_Interrupt----------------------------------------------------

//Uncomment this line to provide a microseconds resolution timer.
#define HL_STEPPER_TIMER

#ifdef HL_STEPPER_TIMER

#define set_stepper_int_period(period_us) OCR5A = period_us<<1;

#define enable_stepper_interrupt() TIMSK5 = (1 << OCIE5A);  // enable timer compare interrupt

#define disable_stepper_interrupt() TIMSK5 = 0;

#define enable_stepper_timer() PRR1 &= B11011111;

#define disable_stepper_timer() PRR1 |= B00100000;

void set_stepper_int_function(void (*f)());

void setup_stepper_interrupt(void (*function)(void), unsigned int period_us);

#endif

//-----------------------------------Control_loops_Milliseconds_Timer_Interrupt-----------------------------------------


//Uncomment this line to provide Milliseconds timers for control loops
#define HL_CONTROL_LOOP_TIMERS


#ifdef HL_STEPPER_TIMER

/* The function to enable a timer interrupt, calling the given function every given period
 * void en_timer_int_i(void (*function)(void), int period_ms);
 */

void enable_loop_interrupt_0(void (*function)(void), unsigned int period_ms);

void enable_loop_interrupt_1(void (*function)(void), unsigned int period_ms);


/* The function to disable a timer interrupt
 * void dis_timer_int_i();
 */

#define disable_loop_interrupt_0() TIMSK3 = 0;


#define disable_loop_interrupt_1() TIMSK4 = 0;

#endif



//----------------------------------------------------Serial------------------------------------------------------------

//Uncomment this line to provide Serial communication across the code
#define HL_SERIAL


#ifdef HL_SERIAL

/* The function to call to initialise the serial at a given baudrate
 * void serial_begin(baudrate b);
 */
/*inline static void serial_begin(baudrate b){

}*/
#define serial_begin(b) Serial.begin(b)

/* The function to call to send_packet a byte on the serial
 * void serial_send_byte(char c);
 */
/*inline static void serial_echo_byte(char c){

}*/
#define serial_echo_byte(c) Serial.print(c)

/* The function to call to send_packet a zero terminated (char *) on the serial
 * void serial_echo_str(char *s);
 */
/*inline static void serial_echo_str(char *s){

}*/
#define serial_echo_str Serial.print


/* The function to call to check if bytes are elements in the serial buffer
 * bool serial_available(void)
 */
/*inline static bool serial_available(void){

}*/
#define serial_available() Serial.available()


/* The function to call to read a byte from serial buffer
 * char serial_read(void)
 */
/*inline static char serial_read(){

}*/
#define serial_read() Serial.read()

#endif

#endif //HDWGGABSTRACTION_H
