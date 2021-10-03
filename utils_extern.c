/*		This file is similar to the utils.c file we've
			been using all year, except it pertains specifically
			to the external hardware I've added. This includes
			six push-buttons and five LEDs. It handles initialization
			of these items and has some simple functions to turn
			LEDs on and off.
*/


#include <MK64F12.h>
#include "utils_extern.h"
#include "utils.h"

/*
		Function that initializes external LEDs.
*/
void LED_ExInit(void) {

  SIM->SCGC5    |= (1 << 11);  // Enable Clock to Port C

	PORTC->PCR[5] = (1 <<  8) ;  // Pin PTC5 is GPIO
	PORTC->PCR[7] = (1 <<  8) ;  // Pin PTC7 is GPIO
	PORTC->PCR[0] = (1 <<  8) ;  // Pin PTC0 is GPIO
	PORTC->PCR[8] = (1 <<  8) ;  // Pin PTC8 is GPIO
	PORTC->PCR[1] = (1 <<  8) ;  // Pin PTC1 is GPIO

  PTC->PDDR = (1 << 5 | 1 << 7 | 1 << 0 | 1 << 8 | 1 << 1);  // enable PTC pins as output
	
	PTC->PCOR   = 1 << 5;   //white LED off
	PTC->PCOR   = 1 << 7;   //yellow LED off
	PTC->PCOR   = 1 << 0;   //red LED off
	PTC->PCOR   = 1 << 8;   //blue LED off
	PTC->PCOR   = 1 << 1;   //green LED off
}


/*
		Function that turns on external white LED.
*/
void White_On(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PSOR   = 1 << 5;   //white LED on
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns off external white LED.
*/
void White_Off(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PCOR   = 1 << 5;   //white LED off
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns on external yellow LED.
*/
void Yellow_On(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PSOR   = 1 << 7;   //yellow LED on
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns off external yellow LED.
*/
void Yellow_Off(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PCOR   = 1 << 7;   //yellow LED off
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns on external red LED.
*/
void Red_On(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PSOR   = 1 << 0;   //red LED on
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns off external red LED.
*/
void Red_Off(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PCOR   = 1 << 0;   //red LED off
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns on external blue LED.
*/
void Blue_On(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PSOR   = 1 << 8;   //blue LED on
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns off external blue LED.
*/
void Blue_Off(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PCOR   = 1 << 8;   //blue LED off
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns on external green LED.
*/
void Green_On(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PSOR   = 1 << 1;   //green LED on
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that turns off external green LED.
*/
void Green_Off(void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTC->PCOR   = 1 << 1;   //green LED off
	
	// Restore interrupts
	__set_PRIMASK(m);
}


/*
		Function that initializes buttons.
*/
void Button_Init(void) {
	//clock already enabled for Port B and Port C
	
	PORTC->PCR[12] = (1 <<  8 | 1 << 1);  // Pin PTC12 is GPIO, enable internal resistor
	PORTC->PCR[3] = (1 <<  8 | 1 << 1);  // Pin PTC3 is GPIO, enable internal resistor
	PORTC->PCR[2] = (1 <<  8 | 1 << 1);  // Pin PTC2 is GPIO, enable internal resistor
	PORTB->PCR[23] = (1 <<  8 | 1 << 1);  // Pin PTB23 is GPIO, enable internal resistor
	PORTB->PCR[9] = (1 <<  8 | 1 << 1);  // Pin PTB9 is GPIO, enable internal resistor
	PORTB->PCR[18] = (1 <<  8 | 1 << 1);  // Pin PTB18 is GPIO, enable internal resistor
	
	PORTC->PCR[12] &= 0b11111111111111111111111111111110;  //select pulldown resistor
	PORTC->PCR[3] &= 0b11111111111111111111111111111110;  //select pulldown resistor
	PORTC->PCR[2] &= 0b11111111111111111111111111111110;  //select pulldown resistor
	PORTB->PCR[23] &= 0b11111111111111111111111111111110;  //select pulldown resistor
	PORTB->PCR[9] &= 0b11111111111111111111111111111110;  //select pulldown resistor
	PORTB->PCR[18] &= 0b11111111111111111111111111111110;  //select pulldown resistor

  PTC->PDDR &= 0b11111111111111111110111111110011;  // enable PTC3/2 as inputs
	PTB->PDDR &= 0b11111111011110111111110111111111;  // enable PTB23/9/18 as inputs
}
