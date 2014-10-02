/*
 * ClockMeister2100.c
 *
 * Created: 2-10-2014 16:13:26
 *  Author: Ariejan
 */ 


#include <avr/io.h>
#include "lcd-routines.h"

int main(void)
{
	lcd_init();
	
	lcd_setcursor(0, 0);
	lcd_string("  Hello");
	
	lcd_setcursor(0, 1);
	lcd_string("world!");
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
	
	return 0;
}