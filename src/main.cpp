#include <Arduino.h>
#include <avr/sleep.h>

#include "main.hpp"
#include "rtc.hpp"
#include "nixie.hpp"

#define LEFT_NIXIE 7
#define RIGHT_NIXIE 8
#define NIXIE_ENABLE 6

#define TILT_PIN A0

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

bool twelveHour = true;

volatile bool topButtonPressed = false;
volatile bool bottomButtonPressed = false;

void setup()
{
	// enable internal pull-up resistors on digital pins for interrupts
	pinMode(A0, INPUT_PULLUP);
	pinMode(2, INPUT_PULLUP);
	pinMode(3, INPUT_PULLUP);

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
	
	delay(500);

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

// top button starts the time setting menu
void handle_top_button_press(void)
{
	// debouncing data for both buttons
	DebouncingData debounceDataBottom;
	DebouncingData debounceDataTop;

	const int DELAY = 200; // a small delay is added between the user presing both buttons to prevent them from prematurely setting the next value

	digitalWrite(LEFT_NIXIE, LOW);
	digitalWrite(RIGHT_NIXIE, LOW);
	digitalWrite(NIXIE_ENABLE, HIGH);

	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	
	// enable either 12 or 24 mode
	while (digitalRead(2) != LOW || digitalRead(3) != LOW)
	{
		if (debounced_digital_read(&debounceDataTop, 2) == LOW)
		{
			twelveHour = true;

		}

		if (debounced_digital_read(&debounceDataBottom, 3) == LOW)
		{
			twelveHour = false;

		}

		if (twelveHour)
		{
			pulse_nixies(10, 1, 2);
		}
		else
		{
			pulse_nixies(10, 2, 4);
		}		
	}
	
	while (digitalRead(2) == LOW || digitalRead(3) == LOW)
	{
		continue;
	}

	// set hours
	const int HOURS = twelveHour ? 12 : 24;
	while (digitalRead(3) != LOW || digitalRead(2) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, 3) == LOW)
		{
			if (hours + 1 < HOURS)
			{
				hours++;
			}
		}

		if (debounced_digital_read(&debounceDataTop, 2) == LOW)
		{
			if (hours - 1 >= 0)
			{
				hours--;
			}
		}
		pulse_nixies(10, (int)(hours / 10), hours % 10);
	}

	while (digitalRead(2) == LOW || digitalRead(3) == LOW)
	{
		continue;
	}

	// set minutes
	while (digitalRead(3) != LOW || digitalRead(2) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, 3) == LOW)
		{
			if (minutes + 1 < 60)
			{
				minutes++;
			}
		}

		if (debounced_digital_read(&debounceDataTop, 2) == LOW)
		{
			if (minutes - 1 >= 0)
			{
				minutes--;
			}

		}
		pulse_nixies(10, (int)(minutes / 10), minutes % 10);
	}

	while (digitalRead(2) == LOW || digitalRead(3) == LOW)
	{
		continue;
	}

	// set seconds
	while (digitalRead(3) != LOW || digitalRead(2) != LOW)
	{
		if (debounced_digital_read(&debounceDataBottom, 3) == LOW)
		{
			if (seconds + 1 < 60)
			{
				seconds++;
			}
		}

		if (debounced_digital_read(&debounceDataTop, 2) == LOW)
		{
			if (seconds - 1 >= 0)
			{
				seconds--;
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
	digitalWrite(NIXIE_ENABLE, HIGH);
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
	digitalWrite(NIXIE_ENABLE, LOW);

	delay(100);

	pulse_nixies(500, (int)(hours / 10), hours % 10);
	delay(100);
	pulse_nixies(500, (int)(minutes / 10), minutes % 10);
	delay(100);
	pulse_nixies(500, (int)(seconds / 10), seconds % 10);

	digitalWrite(NIXIE_ENABLE, LOW);
	topButtonPressed = false;
	return;
}

void handle_bottom_button_press(void)
{
	// handle_tilt();
	bottomButtonPressed = false;
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

	return;
}

// flashes both tubes for set duration
void pulse_nixies(unsigned long milliseconds, int leftDigit, int rightDigit)
{
	if (leftDigit < 0 || rightDigit < 0 || leftDigit > 9 || rightDigit > 9)
	{
		return;
	}

	digitalWrite(NIXIE_ENABLE, HIGH);
	unsigned long startTime = millis();
	
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
	digitalWrite(NIXIE_ENABLE, LOW);
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