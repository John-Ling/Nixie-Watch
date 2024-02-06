#include "utils.hpp"
// returns an integer that corresponds to the 3 ports 
// 0: PINB, 1: PINC, 2: PIND
// -1: invalid

#define CPU_FREQ 1000000
volatile unsigned long elapsedMillis;

int pin_to_port(int pin)
{
	if ((pin < 0 || pin > 13) && (pin < PC0 || pin > PC5))
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

// should be called before calling millis()
// interrupts should be enable using sei() after calling this method
void init_millis(void)
{
	// I'll be real with you I don't fully understand why this is
	// but it sets up a timer interrupt to increment a get_milliseconds variable
	// then we can pull the value of it to get how many get_milliseconds has passed since execution

	unsigned long ctcMatchOverflow = (CPU_FREQ / 1000) / 8; // isr is executed each time this number is reached this number is ~1ms
	TCCR1B |= (1 << WGM12) | (1 << CS11); // set prescaler to 8
	OCR1AH = ctcMatchOverflow >> 8; 
	OCR1AL = ctcMatchOverflow; 
	TIMSK1 |= (1 << OCIE1A); // enable compare match interrupt
	return;
}

unsigned long millis(void)
{
	cli();
	unsigned long m = elapsedMillis;
	sei();
	return m;
}

ISR(TIMER1_COMPA_vect)
{
	elapsedMillis++;
}

// display digit using bcd encoder
void display_digit(int digit)
{
	// reset outputs
	PORTC = 0x1;
    switch (digit)
    {
        case 0:
            PORTC |= 0b00000011;
            break;
        case 1:
            PORTC |= 0b00100011;
            break;
        case 2:
            PORTC |= 0b00010011;
            break;
        case 3:
            PORTC |= 0b00110011;
            break;
        case 4:
            PORTC |= 0b00001011;
            break;
        case 5:
            PORTC |= 0b00101011;
            break;
        case 6:
            PORTC |= 0b00011011;
            break;
        case 7:
            PORTC |= 0b00111011;
            break;
        case 8:
            PORTC |= 0b00000111;
            break;
        case 9:
            PORTC |= 0b00100111;
            break;
        default:
            return;
    }
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