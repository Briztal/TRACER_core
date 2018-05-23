#ifndef TRACER_ARCH_TEENSY35_H
#define TRACER_ARCH_TEENSY35_H

#include "Kernel/arch/processors/arm_cortex_m4f.h"

/*
 * --------------------------------------- PORT ---------------------------------------
 */

//We will use the kinetis PORT module;
#include <Kernel/arch/peripherals/kinetis/kinetis_PORT.h>

//Declare A, B, C, D and E ports;
KINETIS_PORT_DECLARE(A);
KINETIS_PORT_DECLARE(B);
KINETIS_PORT_DECLARE(C);
KINETIS_PORT_DECLARE(D);
KINETIS_PORT_DECLARE(E);


/*
 * --------------------------------------- UARTS ---------------------------------------
 */

//To define UART drivers we must include the UART driver header;
#include <Kernel/drivers/UART.h>

//The teensy35 supports 5 UARTS;
KINETIS_UART_DECLARE(0);
KINETIS_UART_DECLARE(1);
KINETIS_UART_DECLARE(2);
KINETIS_UART_DECLARE(3);
KINETIS_UART_DECLARE(4);
KINETIS_UART_DECLARE(5);

#endif