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

volatile bool block = false;
volatile bool topButtonPressed = false;
volatile bool bottomButtonPressed = false;
volatile bool disable = false;

void setup()
{
	// enable internal pull-up resistors on digital pins for interrupts
	digitalWrite(A0, HIGH);
	pinMode(2, INPUT);
	pinMode(3, INPUT);
	digitalWrite(2, HIGH);
	digitalWrite(3, HIGH);

	pinMode(LEFT_NIXIE, OUTPUT);
	pinMode(RIGHT_NIXIE, OUTPUT);
	pinMode(NIXIE_ENABLE, OUTPUT);

	digitalWrite(NIXIE_ENABLE, HIGH);

	// enable pin change interrupts on pin A0 / PCINT8 for watch trigger interrupt
	PCICR |= 0b00000010;
	PCMSK1 |= 0b00000001;

	rtc.set_hours(5);
	rtc.set_minutes(12);
	rtc.set_seconds(0);
}

void loop()
{
	// code here will be executed if the arduino wakes
	if (topButtonPressed)
	{
		// handle_top_button_press();
	}
	else if (bottomButtonPressed)
	{
		// handle_bottom_button_press();
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

void handle_top_button_press(void)
{
	DebouncingData debounceData;

	digitalWrite(RIGHT_NIXIE, LOW);
	digitalWrite(NIXIE_ENABLE, LOW);

	bool state = false;
	while (debounced_digital_read(&debounceData, 3) != LOW)
	{
		// blink left nixie to signify it is the selected value
		static unsigned long startTime = millis();
		if (millis() - startTime > 100)
		{
			state = !state;
			digitalWrite(LEFT_NIXIE, state);
			startTime = millis();
		}
	}
	digitalWrite(LEFT_NIXIE, HIGH);
	state = false;
	while (debounced_digital_read(&debounceData, 3) != LOW)
	{
		// blink left nixie to signify it is the selected value
		static unsigned long startTime = millis();
		if (millis() - startTime > 100)
		{
			state = !state;
			digitalWrite(RIGHT_NIXIE, state);
			startTime = millis();
		}
	}

	digitalWrite(NIXIE_ENABLE, LOW);
	return;
}

void handle_bottom_button_press(void)
{
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
	delay(100);
	return;
}


// test for "time display" will be replaced by function that controls nixie tubes
void blink(void) 
{
	pulse_nixies(500, 0, 2);
	delay(100);
	pulse_nixies(500, 4, 3);
	delay(100);
	pulse_nixies(500, 1, 2);
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
		driver.write_digit(leftDigit);
		digitalWrite(LEFT_NIXIE, HIGH);
		digitalWrite(RIGHT_NIXIE, LOW);
		delay(10);

		// display right nixie
		driver.write_digit(rightDigit);
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