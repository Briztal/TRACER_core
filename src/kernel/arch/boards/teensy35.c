/*
  teensy35.c Part of TRACER

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


#include "teensy35.h"


/*
 * --------------------------------------- PORT ---------------------------------------
 */

//TODO PJRC TRIBUTE

//Define port structs addresses;
#define PORT_A_DATA	(volatile uint32_t *)0x40049000
#define PORT_B_DATA	(volatile uint32_t *)0x4004A000
#define PORT_C_DATA	(volatile uint32_t *)0x4004B000
#define PORT_D_DATA	(volatile uint32_t *)0x4004C000
#define PORT_E_DATA	(volatile uint32_t *)0x4004D000
#define GPIO_A_DATA	(volatile uint32_t *)0x400FF000
#define GPIO_B_DATA	(volatile uint32_t *)0x400FF040
#define GPIO_C_DATA	(volatile uint32_t *)0x400FF080
#define GPIO_D_DATA	(volatile uint32_t *)0x400FF0C0
#define GPIO_E_DATA	(volatile uint32_t *)0x400FF100

//Define ports;
KINETIS_PORT_DEFINE(A, PORT_A_DATA, GPIO_A_DATA);
KINETIS_PORT_DEFINE(B, PORT_B_DATA, GPIO_B_DATA);
KINETIS_PORT_DEFINE(C, PORT_C_DATA, GPIO_C_DATA);
KINETIS_PORT_DEFINE(D, PORT_D_DATA, GPIO_D_DATA);
KINETIS_PORT_DEFINE(E, PORT_E_DATA, GPIO_E_DATA);


/*
 * --------------------------------------- PIT ---------------------------------------
 */

//The teensy35 supports 4 PITs.
KINETIS_PIT_DEFINE(0, 0x40037100, IRQ_PIT_CH0, F_BUS)
KINETIS_PIT_DEFINE(1, 0x40037110, IRQ_PIT_CH1, F_BUS)
KINETIS_PIT_DEFINE(2, 0x40037120, IRQ_PIT_CH2, F_BUS)
KINETIS_PIT_DEFINE(3, 0x40037130, IRQ_PIT_CH3, F_BUS)


/*
 * --------------------------------------- UART ---------------------------------------
 */

//Include the kinetis driver header;
#include <kernel/arch/peripherals/kinetis/kinetis_UART.h>

//The teensy35 supports 6 UARTS;
KINETIS_UART_DEFINE(0, 0x4006A000, F_CPU, 8, 8, IRQ_UART0_STATUS, IRQ_UART0_ERROR)

/*
KINETIS_UART_DEFINE(1, 0x4006B000, F_CPU, 8, 8, IRQ_UART1_STATUS, IRQ_UART1_ERROR)
KINETIS_UART_DEFINE(2, 0x4006C000, F_BUS, 1, 1, IRQ_UART2_STATUS, IRQ_UART2_ERROR)
KINETIS_UART_DEFINE(3, 0x4006D000, F_BUS, 1, 1, IRQ_UART3_STATUS, IRQ_UART3_ERROR)
KINETIS_UART_DEFINE(4, 0x400EA000, F_BUS, 1, 1, IRQ_UART4_STATUS, IRQ_UART4_ERROR)
KINETIS_UART_DEFINE(5, 0x400EB000, F_BUS, 1, 1, IRQ_UART5_STATUS, IRQ_UART5_ERROR)

 */

// --------------------------------------- ERROR HANDLING ---------------------------------------

#include <kernel/systick.h>

void arch_handle_error(const char *msg) {

    arch_blink(50);

}


//Notify that an error occurred;
void arch_blink(uint16_t delay) {

    core_enable_interrupts();

    //Cache a pin configuration for pin C5 (LED);
    PORT_pin_config_t config;

    //Cache the LED pin;
    PORT_pin_t pin = {
            .port_data = &PORT_C,
            .bit_index = 5,
    };

    //Get the current configuration;
    PORT_get_pin_config(&pin, &config);

    //Update the configuration for led blink
    config.mux_channel = 1;
    config.direction = PORT_OUTPUT;
    config.output_mode = PORT_HIGH_DRIVE;

    //Update the configuration;
    PORT_set_pin_configuration(&pin, &config);

    //Create registers structs;
    GPIO_output_registers_t c_registers;

    //Get GPIO output registers for port C;
    PORT_get_GPIO_output_registers(&PORT_C, &c_registers);

    //Create the bitmask for C5;
    uint32_t mask = 1<<5;

    //Indefinately :
    while(true){

        //Turn on the LED;
        GPIO_set_bits(c_registers.set_register, mask);

        //Wait 10 ms;
        systick_wait(delay);

        //Turn off the LED;
        GPIO_clear_bits(c_registers.clear_register, mask);

        //Wait 10 ms;
        systick_wait(delay);

    }

}


//Notify that an error occurred;
void arch_count(size_t count) {

    //Enable all interrupts;
    core_enable_interrupts();

    //Cache a pin configuration for pin C5 (LED);
    PORT_pin_config_t config;

    //Cache the LED pin;
    PORT_pin_t pin = {
            .port_data = &PORT_C,
            .bit_index = 5,
    };

    //Get the current configuration;
    PORT_get_pin_config(&pin, &config);

    //Update the configuration for led blink
    config.mux_channel = 1;
    config.direction = PORT_OUTPUT;
    config.output_mode = PORT_HIGH_DRIVE;

    //Update the configuration;
    PORT_set_pin_configuration(&pin, &config);

    //Create registers structs;
    GPIO_output_registers_t c_registers;

    //Get GPIO output registers for port C;
    PORT_get_GPIO_output_registers(&PORT_C, &c_registers);

    //Create the bitmask for C5;
    uint32_t mask = 1<<5;

    //Indefinately :
    while(true){
        for (size_t c = count;c--;) {


            //Turn on the LED;
            GPIO_set_bits(c_registers.set_register, mask);

            //Wait 10 ms;
            systick_wait(250);

            //Turn off the LED;
            GPIO_clear_bits(c_registers.clear_register, mask);

            //Wait 10 ms;
            systick_wait(250);

        }

        systick_wait(2000);

    }

}


