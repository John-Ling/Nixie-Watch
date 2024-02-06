#ifndef Main_Hpp
#define Main_Hpp

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "utils.hpp"
#include "rtc.hpp"

void pulse_nixies(unsigned long milliseconds, int leftDigit, int rightDigit);
void handle_bottom_button_press(void);
void handle_top_button_press(void);
void handle_tilt(void);
void set_time(void);
void set_date(void);
void enable_nixies(void);
void disable_nixies(void);
void top_button_press(void);
void bottom_button_press(void);

#endif