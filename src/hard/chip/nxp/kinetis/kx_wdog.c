//
// Created by root on 8/22/18.
//

#include "kx_wdog.h"

#include <stdint.h>


//------------------------------------------------------- Macros -------------------------------------------------------

#define STCTRLH_WDOGEN ((uint16_t)(1<<0))

// Watchdog Timer (WDOG)

#define WDOG_STCTRLH		(*(volatile uint16_t *)0x40052000) // Watchdog Status and Control Register High
#define WDOG_STCTRLH_DISTESTWDOG	((uint16_t)0x4000)		// Allows the WDOG's functional test mode to be disabled permanently.
#define WDOG_STCTRLH_BYTESEL(n)		((uint16_t)(((n) & 3) << 12))	// selects the byte to be tested when the watchdog is in the byte test mode.
#define WDOG_STCTRLH_TESTSEL		((uint16_t)0x0800)
#define WDOG_STCTRLH_TESTWDOG		((uint16_t)0x0400)
#define WDOG_STCTRLH_WAITEN		((uint16_t)0x0080)
#define WDOG_STCTRLH_STOPEN		((uint16_t)0x0040)
#define WDOG_STCTRLH_DBGEN		((uint16_t)0x0020)
#define WDOG_STCTRLH_ALLOWUPDATE	((uint16_t)0x0010)
#define WDOG_STCTRLH_WINEN		((uint16_t)0x0008)
#define WDOG_STCTRLH_IRQRSTEN		((uint16_t)0x0004)
#define WDOG_STCTRLH_CLKSRC		((uint16_t)0x0002)
#define WDOG_STCTRLH_WDOGEN		((uint16_t)0x0001)
#define WDOG_STCTRLL		(*(volatile uint16_t *)0x40052002) // Watchdog Status and Control Register Low
#define WDOG_TOVALH		(*(volatile uint16_t *)0x40052004) // Watchdog Time-out Value Register High
#define WDOG_TOVALL		(*(volatile uint16_t *)0x40052006) // Watchdog Time-out Value Register Low
#define WDOG_WINH		(*(volatile uint16_t *)0x40052008) // Watchdog Window Register High
#define WDOG_WINL		(*(volatile uint16_t *)0x4005200A) // Watchdog Window Register Low
#define WDOG_REFRESH		(*(volatile uint16_t *)0x4005200C) // Watchdog Refresh register
#define WDOG_UNLOCK		(*(volatile uint16_t *)0x4005200E) // Watchdog Unlock register
#define WDOG_UNLOCK_SEQ1		((uint16_t)0xC520)
#define WDOG_UNLOCK_SEQ2		((uint16_t)0xD928)
#define WDOG_TMROUTH		(*(volatile uint16_t *)0x40052010) // Watchdog Timer Output Register High
#define WDOG_TMROUTL		(*(volatile uint16_t *)0x40052012) // Watchdog Timer Output Register Low
#define WDOG_RSTCNT		(*(volatile uint16_t *)0x40052014) // Watchdog Reset Count register
#define WDOG_PRESC		(*(volatile uint16_t *)0x40052016) // Watchdog Prescaler register



//------------------------------------------------------ Functions -----------------------------------------------------


/**
 * kx_wdog_unlock : unlocks the watchdog. A precise timing is required, reason why this function is inline;
 */

static inline void kx_wdog_unlock() {

	WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
	WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
	__asm__ __volatile__ ("nop");

}


/**
 * k64_wdog_disable : disables the watchdog;
 */

void k64_wdog_disable() {

	//Unlock the watchdog;
	kx_wdog_unlock();

	//Authorise update to the write only one WDOG registers;
	WDOG_STCTRLH = WDOG_STCTRLH_ALLOWUPDATE;

	//Clear the enable bit in the watchdog;
	WDOG_STCTRLH &= ~STCTRLH_WDOGEN;

}