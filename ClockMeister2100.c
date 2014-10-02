/*
 * ClockMeister2100.c
 *
 * Created: 2-10-2014 16:13:26
 *  Author: Ariejan
 */ 

#define F_CPU 16000000UL // 16 Mhz

#include <avr/io.h>
#include <util/delay.h>
#include "lcd-routines.h"

int main(void)
{
	// Initialize LCD
	lcd_init();
	
	lcd_clear();
	lcd_setcursor(0, 0);
	lcd_string("  Hello");
	lcd_setcursor(0, 1);
	lcd_string("world!");
	
	// Setup input
	DDRC = 0x00; // Set as input
	PORTC = 0b00000110; // Enable pull-up resistors
	
    while(1)
    {
		if ((~PINC & 0x02)) { // Hour set
			lcd_clear();
			lcd_setcursor(0, 0);
			lcd_string(">> HOUR");	
		} else if ((~PINC & 0x04)) { // Minute set
			lcd_clear();
			lcd_setcursor(0, 0);
			lcd_string(">> MIN");
		} else {
			lcd_clear();
			lcd_setcursor(0, 0);
			lcd_string("  Hello");			
			lcd_setcursor(0, 1);
			lcd_string("world!");
		}

		_delay_ms(100);
    }
	
	return 0;
}