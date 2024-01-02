#include <Arduino.h>
#include <avr/sleep.h>

#include "main.hpp"

const int led = 8;
const int led2 = 7;

volatile bool block = false;
volatile bool topButtonPressed = false;
volatile bool bottomButtonPressed = false;

void setup()
{
	digitalWrite(A0, HIGH); // enable pull-up
	digitalWrite(2, HIGH);
	digitalWrite(3, HIGH);

	pinMode(led, OUTPUT);
	pinMode(led2, OUTPUT);

	// enable pin change interrupts on pin A0 / PCINT8
	PCICR |= 0b00000010;
	PCMSK1 |= 0b00000001;
}

void loop()
{
	if (topButtonPressed)
	{
		top_blink();
	}
	else if (bottomButtonPressed)
	{
		bottom_blink();
	}
	else
	{
		blink();
	}
	
	delay(500);

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
	sleep_cpu(); // put arduino back to sleep
}

ISR(PCINT1_vect) 
{
	// these are pin change interrupts meaningt they run on any change i.e if the voltage is high or low
	// this restricts waking the microcontroller to only when the pin is initially grounded 
	// when the pin goes back to high when the pin is disconnected from ground the arduino does not wake up
	if (digitalRead(A0) == LOW) 
	{
		sleep_disable();
	}
}

// test for "time display" will be replaced by function that controls nixie tubes
void blink(void) 
{
	for (int i = 0; i < 3; i++)
	{
		delay(100);
		digitalWrite(led, HIGH);
		delay(100);
		digitalWrite(led, LOW);
	}

	for (int i = 0; i < 3; i++)
	{
		delay(100);
		digitalWrite(led2, HIGH);
		delay(100);
		digitalWrite(led2, LOW);
	}
	return;
}

void top_blink(void) 
{
	for (int i = 0; i < 6; i++)
	{
		delay(50);
		digitalWrite(led, HIGH);
		delay(50);
		digitalWrite(led, LOW);
	}
	topButtonPressed = false;
	return;
}

void bottom_blink(void)
{
	for (int i = 0; i < 6; i++)
	{
		delay(50);
		digitalWrite(led2, HIGH);
		delay(50);
		digitalWrite(led2, LOW);
	}
	bottomButtonPressed = false;
	return;
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