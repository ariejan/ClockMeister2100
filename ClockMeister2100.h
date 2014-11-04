/*
 * ClockMeister2100.h
 *
 * Created: 4-11-2014 22:14:05
 *  Author: Ariejan
 */ 


#ifndef CLOCKMEISTER2100_H_
#define CLOCKMEISTER2100_H_

// Setup methods
void setup();
void setup_i2c();
void setup_rtc();
void setup_temperature();
void setup_lcd();
void setup_buttons();
void setup_analog();

void render_time();
void render_temperature();

int read_hour();
int read_minute();
int read_temperature();

#endif /* CLOCKMEISTER2100_H_ */