#include <Arduino.h>
#include <avr/sleep.h>

#include "main.hpp"
#include "rtc.hpp"

#define LED 0
#define LED_2 8
#define LED_3 7

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
	// enable internal pull-up resistors on digital pins for time-setting buttons
	digitalWrite(A0, HIGH);
	pinMode(2, INPUT);
	pinMode(3, INPUT);
	digitalWrite(2, HIGH);
	digitalWrite(3, HIGH);

	pinMode(LED, OUTPUT);
	pinMode(LED_2, OUTPUT);
	pinMode(LED_3, OUTPUT);

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
		display_time();
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

// test for "time display" will be replaced by function that controls nixie tubes
void blink(void) 
{
	for (int i = 0; i < 3; i++)
	{
		delay(100);
		digitalWrite(LED_2, HIGH);
		delay(100);
		digitalWrite(LED_2, LOW);
	}

	for (int i = 0; i < 3; i++)
	{
		delay(100);
		digitalWrite(LED_3, HIGH);
		delay(100);
		digitalWrite(LED_3, LOW);
	}
	return;
}

void display_time(void)
{
	// int hours = rtc.get_hours();
	// int minutes = rtc.get_minutes();
	// int seconds = rtc.get_seconds();

	// for (int i = 0; i < hours; i++)
	// {
	// 	digitalWrite(LED, HIGH);
	// 	delay(100);
	// 	digitalWrite(LED, LOW);
	// 	delay(100);
	// }

	// for (int i = 0; i < minutes; i++)
	// {
	// 	digitalWrite(LED_2, HIGH);
	// 	delay(100);
	// 	digitalWrite(LED_2, LOW);
	// 	delay(100);
	// }

	// for (int i = 0; i < seconds; i++)
	// {
	// 	digitalWrite(LED_3, HIGH);
	// 	delay(100);
	// 	digitalWrite(LED_3, LOW);
	// 	delay(100);
	// }

	return;

}

void debug_blink(int pin, int count, int pulseDuration)
{
	for (int i = 0; i < count; i++)
	{
		digitalWrite(pin, HIGH);
		delay(pulseDuration);
		digitalWrite(pin, LOW);
		delay(pulseDuration);
	}
	return;
}

void top_blink(void) 
{
	for (int i = 0; i < 6; i++)
	{
		digitalWrite(LED_2, HIGH);
		delay(50);
		digitalWrite(LED_2, LOW);
		delay(50);
	}

	DebouncingData topButtonData;
	DebouncingData bottomButtonData;

	unsigned int pressCount = 0;
	while (digitalRead(3) != LOW || digitalRead(2) != LOW)
	{
		int topButtonRead = debounced_digital_read(&topButtonData, 2);
		int bottomButtonRead = debounced_digital_read(&bottomButtonData, 3);

		if (topButtonRead == 0)
		{
			pressCount++;
			digitalWrite(LED_2, HIGH);
			while (digitalRead(2) == LOW)
			{
				continue;
			}
			topButtonData.previousState = topButtonData.currentState;
		} 
		else if (bottomButtonRead == 0)
		{
			if (pressCount - 1 >= 0)
			{
				pressCount--;
			}
			digitalWrite(LED_2, HIGH);
			while(digitalRead(3) == LOW)
			{
				continue;
			}
			bottomButtonData.previousState = bottomButtonData.currentState;
		}

		digitalWrite(LED_2, LOW);
	}

	for (unsigned int i = 0; i < pressCount; i++)
	{
		delay(500);
		digitalWrite(LED_2, HIGH);
		delay(500);
		digitalWrite(LED_2, LOW);
	}
	topButtonPressed = false;
	disable = false;
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

void bottom_blink(void)
{
	for (int i = 0; i < 6; i++)
	{
		delay(50);
		digitalWrite(LED_3, HIGH);
		delay(50);
		digitalWrite(LED_3, LOW);
	}
	bottomButtonPressed = false;
	return;
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