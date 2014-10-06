/*
 * ClockMeister2100.c
 *
 * Created: 2-10-2014 16:13:26
 *  Author: Ariejan
 */ 

#include "config.h"

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>

#include "lcd.h"
#include "i2c.h"
#include "ds1307.h"

// Initialize I2C
void setup_i2c() {
	I2CInit();
}

void lcd_create_characters() {
	uint8_t timeChar[8] = {
		0b01110,
		0b10101,
		0b10101,
		0b10101,
		0b10111,
		0b10001,
		0b10001,
		0b01110
	};
	
	lcd_generatechar(0x00, timeChar);
	
	uint8_t tempChar[8] = {
		0b00100,
		0b01010,
		0b01010,
		0b01010,
		0b01110,
		0b11111,
		0b11111,
		0b01110
	};
	
	lcd_generatechar(0x01, tempChar);
}

// Initialize LCD for use
void setup_lcd() {
	// Initialize LCD
	lcd_init();

	lcd_create_characters();

	lcd_clear();
	lcd_setcursor(0, 0);
	lcd_string("  ClockM");
	lcd_setcursor(0, 1);
	lcd_string("eister");	
}

// Setup buttons
void setup_buttons() {
	// Set C pint as input.
	DDRC = 0x00; // Set as input
	
	// Enable internal pull-ups on C1 and C2
	PORTC = 0b00000110;	
}

// Setup analog input
void setup_analog() {
	// Set pre-scaler to 128, 127kHz at 16Mhz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// Use AVcc as reference voltage
	ADMUX |= (1 << REFS0);
	
	// Right adjust ADC results for full 10-bit reading
	ADMUX |= (0 << ADLAR);

	// No MUX values needed to be changed to use ADC0

	// Enable ADC
	ADCSRA |= (1 << ADEN);	
}

// Runs once on power-on
void setup() {
	setup_i2c();
	setup_lcd();
	setup_buttons();
	setup_analog();
	_delay_ms(SETUP_DELAY_MS);
}

int bcd_to_dec(int val) {
	return ((val/16*10) + (val%16));
}

int dec_to_bcd(int val) {
	return ((val/10*16) + (val%10));
}

int read_from_ds1307(uint8_t address) {
	uint8_t data;
	DS1307Read(address, &data);
	return bcd_to_dec(data);
}

void write_to_ds1307(uint8_t address, uint8_t value) {
	DS1307Write(address, dec_to_bcd(value));
}

int read_hour() {
	uint8_t data = read_from_ds1307(0x02);
	return data;
}

int read_minute() {
	uint8_t data = read_from_ds1307(0x01);
	return data;
}

void increment_hour() {
	uint8_t value = read_from_ds1307(0x02);
	value = (value + 1) % 24;
	write_to_ds1307(0x02, value);
}

void increment_minute() {
	uint8_t value = read_from_ds1307(0x01);
	value = (value + 1) % 60;
	write_to_ds1307(0x01, value);
}

void render_time() {
	char output[7];
	int hour = read_hour();
	int minute = read_minute();
	
	sprintf(output, " %02d:%02d ", hour, minute);
	
	lcd_setcursor(0, 0);
	lcd_data(0x00);
	lcd_string(output);
}

// Take a 10-bit analog reading and
// return the resulting temperature in tenth's of degree C.
// E.g. 250 => 25.0 C
int read_temperature() {
	// Take a 10-bit analog reading.
	ADCSRA |= (1 << ADSC);
	while (((ADCSRA >> ADSC) & 1)){}
		
	// Requirement: read ADCL before ADCH
	int low = ADCL;
	double reading = (ADCH << 8) | low;
	
	// Convert the reading to mV
	// Given a Aref of 5.0V and 10-bit precision.
	int mvolts = (int)(reading * (5000.0 / 1023.0));
	
	// TMP36 has a base of 500mV, resolution of 10mV per degree C.
	int temperature = (mvolts - 500);
	
	return temperature;
}

void render_temperature() {
	int temperature = read_temperature();
    char output[7];
	
	// Split the full degrees from the decimal to avoid floats
	int degrees = (int)(temperature / 10);
	int decimal = temperature - (degrees * 10);
	
	// Format the output
	sprintf(output, " %2d,%1dC  ", degrees, decimal);
	
	// Output to LCD
	lcd_setcursor(0, 1);
	lcd_data(0x01);
	lcd_string(output);
}

int main(void)
{
	setup();
	
	while(1)
    {
		if ((~PINC & 0x02)) { // Hour set
			increment_hour();
		} else if ((~PINC & 0x04)) { // Minute set
			increment_minute();
		} else {
			render_time();
			render_temperature();
		}
		_delay_ms(150);
    }
	
	return 0;
}