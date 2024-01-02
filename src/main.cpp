#include <Arduino.h>
#include <avr/sleep.h>

#include "main.hpp"

const int led = 8;
const int led2 = 7;

volatile bool block = false;

void setup()
{
	digitalWrite(A0, HIGH); // enable pull-up
	pinMode(led, OUTPUT);
	pinMode(led2, OUTPUT);

	// enable pin change interrupts on pin A0 / PCINT8
	PCICR |= 0b00000010;
	PCMSK1 |= 0b00000001;
}

void loop()
{
	blink();
	delay(500);

	ADCSRA = 0;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// Do not interrupt before we go to sleep, or the
	// ISR will detach interrupts and we won't wake.
	noInterrupts();

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