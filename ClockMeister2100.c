/*
 * ClockMeister2100.c
 *
 * Created: 2-10-2014 16:13:26
 *  Author: Ariejan
 */ 

#define F_CPU 16000000UL // 16 Mhz

#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>
#include "lcd.h"

void render_temperature() {
	lcd_clear();
	lcd_setcursor(0, 0);
	lcd_string("CM-2100");
	lcd_setcursor(0, 1);
			
	// Take a reading.
	ADCSRA |= (1 << ADSC);
	while (((ADCSRA >> ADSC) & 1)){}
	int low = ADCL;
	double value = (ADCH << 8) | low;
			
	// Convert to mV using the 5.0V Vcc reference and 10-bit accuracy
	int mvolts = (int)(value * (5000.0 / 1023.0));
			
	// Calculating tenths of degrees C
	int tmp = mvolts - 500;
			
	// Split degrees C from tenths of degrees C
	int full = tmp / 10;
	int deci = tmp - (full * 10);
			
	// Format and output.
	char t1[3];
	char t2[1];
	itoa(full, t1, 10);
	itoa(deci, t2, 10);
	lcd_string("T: ");
	lcd_string(t1);
	lcd_string(",");
	lcd_string(t2);
	lcd_string("C");	
}

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
	
	// Setup analog input
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ACD prescalar to 128, 125kHz sample rate at 16Mhz
	ADMUX |= (1 << REFS0); // Set reference voltage to AVCC (TODO: Get 5V from somewhere else?)
	ADMUX |= (0 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

	// No MUX values needed to be changed to use ADC0

	ADCSRA |= (1 << ADEN);  // Enable ADC	
	
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
			render_temperature();
		}
		_delay_ms(200);
    }
	
	return 0;
}