//
// Created by root on 5/21/18.
//

#ifndef TRACER_PORT_H
#define TRACER_PORT_H

/*
 * The port module manages the pins behaviour.
 *
 *  It manages :
 *  - Interrupts and DMA modes for pins interrupts;
 *  - Pin multiplexing;
 *  - Hardware configuration for both data directions;
 */

#include "stdint.h"

#include "Kernel/arch/arch.h"

/*
 * Types of interruptions;
 */

typedef enum {

    //No interruption enabled;
            PORT_NO_INTERRUPT,

    //DMA request on rising edge;
            PORT_DMA_RISING_EDGE,

    //DMA request on falling edge;
            PORT_DMA_FALLING_EDGE,

    //DMA request on rising or falling edge;
            PORT_DMA_EDGE,

    //Interrupt request when logical 0;
            PORT_INTERRUPT_0,

    //Interrupt request when logical 1;
            PORT_INTERRUPT_1,

    //Interrupt request on rising edge;
            PORT_INTERRUPT_RISING_EDGE,

    //Interrupt request on falling edge;
            PORT_INTERRUPT_FALLING_EDGE,

    //Interrupt request on rising or falling edge;
            PORT_INTERRUPT_EDGE,

} PORT_interrupt_t;


/*
 * The hardware data direction;
 */

typedef enum {

    //Pin is used to collect data;
            PORT_INPUT,

    //Pin is used to provide data;
            PORT_OUTPUT

} PORT_pin_direction_t;


/*
 * Different input hardware configurations;
 */

typedef enum {

    //High impedance configuration;
            PORT_HIGH_IMPEDANCE,

    //Pull-up configuration;
            PORT_PULL_UP,

    //Pull-down configuration;
            PORT_PULL_DOWN,

    //Hysteresis configuration;
            PORT_HYSTERSIS,


    //Repeater configuration;
            PORT_REPEATER,

} PORT_input_mode_t;


/*
 * Different hardware output configurations;
 */

typedef enum {

    //Open drain configuration;
            PORT_OPEN_DRAIN,

    //Push-pull configuration;
            PORT_PUSH_PULL,

    //High current drive configuration;
            PORT_HIGH_DRIVE,

} PORT_output_mode_t;


/*
 * Different slew rates;
 */

typedef enum {

    //Low slew rate;
            PORT_LOW_RATE,

    //High slew rate;
            PORT_HIGH_RATE,

} PORT_slew_rate_t;


/*
 * The complete configuration for a pin;
 */

typedef struct {

    /*
     * Interruption;
     */

    //The type of interruption;
    PORT_interrupt_t interrupt_type;

    //The function to call in case of interrupt selected;
    void (*interrupt_function)(void);

    /*
     * Hardware configuration;
     */

    //The enabled multiplexing channel;
    uint8_t mux_channel;

    //The data direction;
    PORT_pin_direction_t direction;


    /*
     * Input hardware configuration
     */

    //The input mode;
    PORT_input_mode_t input_mode;

    //The input filter type;
    PORT_input_filter_data_t input_filter;


    /*
     * Output hardware configuration
     */

    //The output mode;
    PORT_output_mode_t output_mode;

    //The slew rate;
    PORT_slew_rate_t slew_rate;

} PORT_pin_config_t;


/*
 * To transmit data through GPIOs, the specialised registers must be known.
 *
 *  They will be transmitted through this struct;
 */

typedef struct {

    //The output data register. Used to update the entire port;
    volatile void *data_register;

    //The set register. Used to set bits in the port;
    volatile void *set_register;

    //The clear register. Used to clear bits in the port;
    volatile void *clear_register;

    //The toggle register. Used to toggle bits in the port;
    volatile void *toggle_register;

} GPIO_output_registers_t;


#include "Kernel/arch/arch_types.h"


/*
 * ---------------------------- Configuration ----------------------------
 */

//Get a pin's current configuration (avoid defaults mistakes);
void PORT_get_pin_config(PORT_data_t *port, uint8_t bit, PORT_pin_config_t *);

//Set a pin's configuration. Config might have been queried before with the function behind;
void PORT_set_pin_configuration(PORT_data_t *port, uint8_t bit, PORT_pin_config_t *);


/*
 * ---------------------------- GPIO ----------------------------
 */


/*
 * GPIO functions are critical functions, that must task the least time as possible.
 *
 *  To enable the hardware abstraction, without loss of efficiency, the control of a GPIO pin is done by the
 *  following :
 *  - Get hardware registers for the required GPIO pin;
 *  - Call a GPIO function, providing the register.
 *
 *  GPIO functions are inline, to enable full optimisation, and are trivial (register assignment) if GPIO functions
 *  (data set, bits set, bits clear, bits toggle) are supported by the hardware;
 *
 *  This method is a bit heavy to setup, but guarantees the best optimisation, regarding the hardware abstraction;
 */


//Get a GPIO pin's hardware registers. This function is used for the setup, and is not made to be fast or optimised;
void PORT_get_GPIO_registers(PORT_data_t *port, uint8_t bit, GPIO_output_registers_t *);

/*
 * GPIO functions are time critical, and so are inline. If your hardware supports GPIO action, you may define
 * GPIO inline functions in your PORT driver's header, and include it in yout arch's header.
 *
 * To ease inter-compatibility, please use function signatures as listed below.
 *

//Define the GPIO mask type to be of your register length;
typedef x GPIO_mask_t;

//Define GPIO hardware register;
typedef volatile GPIO_mask_t *GPIO_hw_register;


//Determine the mask from a bit index;
static inline GPIO_mask_t GPIO_get_mask(uint8_t bit);


// ---------------------------------- GPIO functions ----------------------------------

//The software setter : no software process to do, all is supported by the hardware;
static inline void GPIO_set(volatile void *hw, GPIO_mask_t value);


//The software bitwise setter : no software process to do, all is supported by the hardware;
static inline void GPIO_set_bits(volatile void *hw, GPIO_mask_t value);


//The software bitwise clearer : no software process to do, all is supported by the hardware;
static inline void GPIO_clear_bits(volatile void *hw, GPIO_mask_t value);


//The software bitwise toggler : no software process to do, all is supported by the hardware;
static inline void GPIO_toggle_bits(volatile void * hw, GPIO_mask_t value);

*/

#endif //TRACER_PORT_H