#include <Arduino.h>
#include <avr/sleep.h>

#include "main.hpp"
#include "rtc.hpp"

#define LEFT_NIXIE 7
#define RIGHT_NIXIE 8
#define NIXIE_DISABLE 6

#define TILT_PIN A0

// rtc pins
#define CE 9
#define IO 10
#define SCLK 11

RTC rtc(CE, IO, SCLK);

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
	pinMode(NIXIE_DISABLE, OUTPUT);
	digitalWrite(NIXIE_DISABLE, HIGH);

	pinMode(A1, OUTPUT);
	pinMode(A2, OUTPUT);
	pinMode(A3, OUTPUT);

	digitalWrite(A3, HIGH);
	digitalWrite(NIXIE_DISABLE, LOW);
	digitalWrite(LEFT_NIXIE, HIGH);

	// enable pin change interrupts on pin A0 / PCINT8 for watch trigger interrupt
	PCICR |= 0b00000010;
	PCMSK1 |= 0b00000001;

	rtc.set_hours(5);
	rtc.set_minutes(12);
	rtc.set_seconds(10);

}

void loop()
{
	// code here will be executed if the arduino wakes
	// blink();
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


void handle_top_button_press(void)
{
	blink();
	return;
}

void handle_bottom_button_press(void)
{
	return;
}

void handle_tilt(void)
{
	blink();
	return;
}

// test for "time display" will be replaced by function that controls nixie tubes
void blink(void) 
{
	pulse_nixies(500, A3, A1);
	return;
}

// flashes both nixie tubes for set duration does not deactivate power supply
void pulse_nixies(unsigned long milliseconds, int leftDigit, int rightDigit)
{
	digitalWrite(NIXIE_DISABLE, LOW);
	unsigned long startTime = millis();

	while (millis() - startTime < milliseconds)
	{
		digitalWrite(leftDigit, HIGH);
		digitalWrite(rightDigit, LOW);
		// display left nixie
		digitalWrite(LEFT_NIXIE, HIGH);
		digitalWrite(RIGHT_NIXIE, LOW);
		delay(10);

		// display right nixie
		digitalWrite(leftDigit, LOW);
		digitalWrite(rightDigit, HIGH);
		digitalWrite(LEFT_NIXIE, LOW);
		digitalWrite(RIGHT_NIXIE, HIGH);
		delay(10);
	}

	digitalWrite(LEFT_NIXIE, LOW);
	digitalWrite(RIGHT_NIXIE, LOW);
	digitalWrite(leftDigit, LOW);
	digitalWrite(rightDigit, LOW);

	digitalWrite(NIXIE_DISABLE, HIGH);
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
	disable = true;
	return;
}

void bottom_button_press(void)
{
	if (disable)
	{
		return;
	}

	sleep_disable();
	detachInterrupt(1);
	bottomButtonPressed = true;
	return;
}