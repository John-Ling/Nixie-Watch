#include <Arduino.h>
#include "helpers.hpp"

// returns an integer that corresponds to the 3 ports 
// 0: PINB, 1: PINC, 2: PIND
// -1: invalid
int pin_to_port(int pin)
{
	if ((pin < 0 || pin > 13) && (pin < A0 || pin > A5))
	{
		return -1;
	}

	if (pin <= 7)
	{
		return 2;
	}

	if (pin <= 13)
	{
		return 0;
	}

	return 1;

}

int debounced_digital_read(DebouncingData *buttonData, int pin)
{
	const unsigned long DEBOUNCE_DELAY = 20;
	int shiftAmount = pin % 8;
	int readState;
	switch(pin_to_port(pin))
	{
		case 0:
			readState = PINB & (1 << shiftAmount);
			break;
		case 1:
			readState = PINC & (1 << shiftAmount);
			break;
		case 2:
			readState = PIND & (1 << shiftAmount);
			break;
		default:
			return -1;
	}

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