#include <Arduino.h>
#include <avr/sleep.h>
#include <EEPROM.h>

#include "main.hpp"
#include "rtc.hpp"
#include "nixie.hpp"

#define LEFT_NIXIE 7
#define RIGHT_NIXIE 8
#define NIXIE_ENABLE 6

#define TILT_PIN A0

#define TOP_BUTTON 2
#define BOTTOM_BUTTON 3

// rtc pins
#define CE 9
#define IO 10
#define SCLK 11

// bcd encoder inputs
#define A 0
#define B 1
#define C 4
#define D 5

RTC rtc(CE, IO, SCLK);
Nixie_Driver driver(A, B, C, D);

volatile bool topButtonPressed = false;
volatile bool bottomButtonPressed = false;
volatile bool disableInterrupts = false;

void setup()
{
	// enable internal pull-up resistors on digital pins for interrupts
	pinMode(TILT_PIN, INPUT_PULLUP);
	pinMode(TOP_BUTTON, INPUT_PULLUP);
	pinMode(BOTTOM_BUTTON, INPUT_PULLUP);

	pinMode(LEFT_NIXIE, OUTPUT);
	pinMode(RIGHT_NIXIE, OUTPUT);
	pinMode(NIXIE_ENABLE, OUTPUT);

	// enable pin change interrupts on pin A0 / PCINT8 for watch trigger interrupt
	PCICR |= 0b00000010;
	PCMSK1 |= 0b00000001;
}

void loop()
{
	// code here will be executed if the arduino wakes
	if (topButtonPressed)
	{
		handle_top_button_press();
	}
	else if (bottomButtonPressed)
	{
		handle_bottom_button_press();
	}
	else
	{
		handle_tilt();
	}

	delay(100);
	topButtonPressed = false;
	bottomButtonPressed = false;

	// put arduino back to sleep
	ADCSRA = 0;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// Do not interrupt before we go to sleep, or the
	// ISR will detach interrupts and we won't wake.
	noInterrupts();
	attachInterrupt(0, top_button_press, FALLING);
	attachInterrupt(1, bottom_button_press, FALLING);
	EIFR = bit (INTF0);  // clear flag for interrupt 0
	EIFR = bit (INTF1);  // clear flag for interrupt 1
	interrupts(); // re-enable interrupts
	sleep_cpu();
}

ISR(PCINT1_vect) 
{
	// this restricts waking the microcontroller to only when the pin is initially grounded 
	// when the pin goes back to high when the pin is disconnected from ground the arduino does not wake up
	if (digitalRead(A0) == LOW) 
	{
		sleep_disable();
	}
}

// top button starts the time and date setting menu
void handle_top_button_press(void)
{
	set_time();
	set_date();
	return;
}

void handle_bottom_button_press(void)
{
	int day = rtc.get_day();
	pulse_nixies(500, (int)(day / 10), day % 10);
	delay(100);

	int month = rtc.get_month();
	pulse_nixies(500, (int)(month / 10), month % 10);
	delay(100);

	int year = rtc.get_year();
	pulse_nixies(500, (int)(year / 10), year % 10);

	return;
}

void handle_tilt(void)
{
	int hours = rtc.get_hours();

	pulse_nixies(500, (int)(hours / 10), hours % 10);
	delay(100);

	int minutes = rtc.get_minutes();
	pulse_nixies(500, (int)(minutes / 10), minutes % 10);
	delay(100);

	int seconds = rtc.get_seconds();
	pulse_nixies(500, (int)(seconds / 10), seconds % 10);

	return;
}

void enable_nixies(void)
{
	digitalWrite(NIXIE_ENABLE, HIGH);
	return;
}

void disable_nixies(void)
{
	digitalWrite(NIXIE_ENABLE, LOW);
	return;
}

// flashes both tubes for set duration
void pulse_nixies(unsigned long milliseconds, int leftDigit, int rightDigit)
{
	if (leftDigit < 0 || rightDigit < 0 || leftDigit > 9 || rightDigit > 9)
	{
		return;
	}

	enable_nixies();
	unsigned long startTime = millis();
	
	// digits are displayed via multiplexing so switching is required
	while (millis() - startTime < milliseconds)
	{
		// display left nixie
		driver.display_digit(leftDigit);
		digitalWrite(LEFT_NIXIE, HIGH);
		digitalWrite(RIGHT_NIXIE, LOW);
		delay(10);

		// display right nixie
		driver.display_digit(rightDigit);
		digitalWrite(LEFT_NIXIE, LOW);
		digitalWrite(RIGHT_NIXIE, HIGH);
		delay(10);
	}

	digitalWrite(LEFT_NIXIE, LOW);
	digitalWrite(RIGHT_NIXIE, LOW);
	disable_nixies();
	return;
}

void set_time(void)
{
	DebouncingData debounceDataBottom;
	DebouncingData debounceDataTop;

	digitalWrite(LEFT_NIXIE, LOW);
	digitalWrite(RIGHT_NIXIE, LOW);
	enable_nixies();

	int hours = rtc.get_hours();
	int minutes = rtc.get_minutes();
	int seconds = rtc.get_seconds();
	
	while (digitalRead(TOP_BUTTON) == LOW || digitalRead(BOTTOM_BUTTON) == LOW)
	{
		continue;
	}

	// set hours
	const int MAX_HOURS = 24;
	while (digitalRead(BOTTOM_BUTTON) != LOW || digitalRead(TOP_BUTTON) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) == LOW)
		{
			if (hours - 1 >= 0) 
			{
				hours--; 
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) == LOW)
		{
			if (hours + 1 < MAX_HOURS)
			{
				hours++;
			}
		}
		pulse_nixies(10, (int)(hours / 10), hours % 10);
	}

	while (digitalRead(TOP_BUTTON) == LOW || digitalRead(BOTTOM_BUTTON) == LOW)
	{
		continue;
	}

	// set minutes
	while (digitalRead(BOTTOM_BUTTON) != LOW || digitalRead(TOP_BUTTON) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) == LOW)
		{
			if (minutes - 1 >= 0)
			{
				minutes--;
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) == LOW)
		{
			if (minutes + 1 < 60)
			{
				minutes++;
			}
		}
		pulse_nixies(10, (int)(minutes / 10), minutes % 10);
	}

	while (digitalRead(TOP_BUTTON) == LOW || digitalRead(BOTTOM_BUTTON) == LOW)
	{
		continue;
	}

	// set seconds
	while (digitalRead(BOTTOM_BUTTON) != LOW || digitalRead(TOP_BUTTON) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) == LOW)
		{
			if (seconds - 1 >= 0)
			{
				seconds--;
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) == LOW)
		{
			if (seconds + 1 < 60)
			{
				seconds++;
			}	
		}

		pulse_nixies(10, (int)(seconds / 10), seconds % 10);
	}

	rtc.set_hours(hours);
	rtc.set_minutes(minutes);
	rtc.set_seconds(seconds);

	unsigned int startTime = millis();
	digitalWrite(LEFT_NIXIE, HIGH);
	digitalWrite(RIGHT_NIXIE, HIGH);
	enable_nixies();

	while (millis() - startTime <= 2000)
	{
		for (int i = 0; i < 10; i++)
		{
			driver.display_digit(i);
			delay(50);
		}
	}
	
	digitalWrite(LEFT_NIXIE, LOW);
	digitalWrite(RIGHT_NIXIE, LOW);
	disable_nixies();

	delay(100);

	pulse_nixies(500, (int)(hours / 10), hours % 10);
	delay(100);
	pulse_nixies(500, (int)(minutes / 10), minutes % 10);
	delay(100);
	pulse_nixies(500, (int)(seconds / 10), seconds % 10);
	return;
}

void set_date(void)
{
	DebouncingData debounceDataTop;
	DebouncingData debounceDataBottom;

	digitalWrite(LEFT_NIXIE, LOW);
	digitalWrite(RIGHT_NIXIE, LOW);
	enable_nixies();

	int year = rtc.get_year();

	// set year
	while (digitalRead(BOTTOM_BUTTON) != LOW || digitalRead(TOP_BUTTON) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) != LOW)
		{
			if (year - 1 >= 0)
			{
				year--;
			}	
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) != LOW)
		{
			if (year + 1 < 100)
			{
				year++;
			}
		}
		pulse_nixies(10, (int)(year / 10), year % 10);
	}

	bool leapYear = year % 4 == 0 ? true : false;

	while (digitalRead(TOP_BUTTON) == LOW || digitalRead(BOTTOM_BUTTON) == LOW)
	{
		continue;
	}
	
	int month = rtc.get_month();
	// set month
	while (digitalRead(BOTTOM_BUTTON) != LOW || digitalRead(TOP_BUTTON) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) != LOW)
		{
			if (month - 1 >= 0)
			{
				month--;
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) != LOW)
		{
			if (month + 1 < 13)
			{
				month++;
			}	
		}
		pulse_nixies(10, (int)(month / 10), month % 10);
	}

	while (digitalRead(TOP_BUTTON) == LOW || digitalRead(BOTTOM_BUTTON) == LOW)
	{
		continue;
	}

	int day = rtc.get_day();
	const int MAX_DAYS = (month == 2 && leapYear) ? 29 : (month == 2) ? 28 : (month == 9 || month == 4 || month == 6 || month == 11) ? 30 : 31;

	// set day
	while (digitalRead(BOTTOM_BUTTON) != LOW || digitalRead(TOP_BUTTON) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) != LOW)
		{
			if (day - 1 >= 0)
			{
				day--;
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) != LOW)
		{
			if (day + 1 <= MAX_DAYS)
			{
				day++;
			}
		}
		pulse_nixies(10, (int)(day / 10), day % 10);
	}

	rtc.set_day(day);
	rtc.set_month(month);
	rtc.set_year(year);

	unsigned int startTime = millis();
	digitalWrite(LEFT_NIXIE, HIGH);
	digitalWrite(RIGHT_NIXIE, HIGH);
	enable_nixies();

	while (millis() - startTime <= 2000)
	{
		for (int i = 0; i < 10; i++)
		{
			driver.display_digit(i);
			delay(50);
		}
	}
	
	digitalWrite(LEFT_NIXIE, LOW);
	digitalWrite(RIGHT_NIXIE, LOW);
	disable_nixies();
	delay(100);

	pulse_nixies(500, (int)(day / 10), day % 10);
	delay(100);
	pulse_nixies(500, (int)(month / 10), month % 10);
	delay(100);
	pulse_nixies(500, (int)(year / 10), year % 10);

	disable_nixies();
	return;
}

int debounced_digital_read(DebouncingData *buttonData, int pin)
{
	const int DEBOUNCE_DELAY = 20;
	int readState = digitalRead(pin);

	if (readState != buttonData->previousState)
	{
		buttonData->previousDebounceTime = millis();
	}

	if ((millis() - buttonData->previousDebounceTime) <= DEBOUNCE_DELAY)
	{
		buttonData->previousState = readState;
		return -1;
	}

	if (readState == buttonData->currentState) 
	{
		buttonData->previousState = readState;
		return -1;
	}

	buttonData->currentState = readState;
	return readState;
}

void top_button_press(void)
{
	sleep_disable();
	detachInterrupt(0);
	topButtonPressed = true;
	return;
}

void bottom_button_press(void)
{
	sleep_disable();
	detachInterrupt(1);
	bottomButtonPressed = true;
	return;
}