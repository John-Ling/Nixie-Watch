#include "main.hpp"

#define LEFT_NIXIE 5
#define RIGHT_NIXIE 7
#define NIXIE_ENABLE 6

#define TILT_PIN A0

#define TOP_BUTTON 2
#define BOTTOM_BUTTON 3

#define PULSE_DELAY 500

volatile bool topButtonPressed = false;
volatile bool bottomButtonPressed = false;

int main(void)
{
	// set registers for tilt switch and encoder inputs
	DDRC = 0b00111101;
	PORTC = 0b00000001;

	// set registers for left and right nixies, nixie enable and time setting buttons
	DDRD = 0b11100000;
	PORTD = 0b00001100;

	// set registers for rtc
	DDRB = 0b00001010;
	PORTB = 0x00;

	// enable pin change interrupts on pin A0 / PCINT8 for watch trigger interrupt
	PCICR |= 0b00000010;
	PCMSK1 |= 0b00000001;
	
	init_millis(); // run this so we can use the millis function
	sei();

	while (1)
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

		_delay_ms(500);
		topButtonPressed = false;
		bottomButtonPressed = false;

		// put arduino back to sleep
		ADCSRA = 0;
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();

		// Do not interrupt before we go to sleep, or the
		// ISR will detach interrupts and we won't wake.
		cli();
		// enable falling interrupts on both INT0 and INT1
		EICRA |= (1 << ISC01) | (1 << ISC11);
		EICRA &= ~((1 << ISC00) | (1 << ISC10));
		EIFR |= (1 << INT0) | (1 << INT1);  // clear external interrupt flags
		EIMSK |= (1 << INT0) | (1 << INT1); // enable interrupts
		sei(); // re-enable interrupts
		sleep_cpu();
	}
}

ISR(INT0_vect)
{
	sleep_disable();
	EIMSK &= ~(1 << INT0); // disable INT0
	topButtonPressed = true;
	return;
}

ISR(INT1_vect)
{
	sleep_disable();
	EIMSK &= ~(1 << INT1); // disable INT1
	bottomButtonPressed = true;
	return;
}

ISR(PCINT1_vect) 
{
	// this restricts waking the microcontroller to only when the pin is initially grounded 
	// when the pin goes back to high when the pin is disconnected from ground the arduino does not wake up
	if ((PINC & 0X01) == 0) 
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
	int day = get_day();
	pulse_nixies(500, (int)(day / 10), day % 10);
	_delay_ms(100);

	int month = get_month();
	pulse_nixies(500, (int)(month / 10), month % 10);
	_delay_ms(100);

	int year = get_year();
	pulse_nixies(500, (int)(year / 10), year % 10);

	return;
}

void handle_tilt(void)
{
	int hours = get_hours();
	pulse_nixies(PULSE_DELAY, (int)(hours / 10), hours % 10);
	_delay_ms(100);

	int minutes = get_minutes();
	pulse_nixies(PULSE_DELAY, (int)(minutes / 10), minutes % 10);
	_delay_ms(100);

	int seconds = get_seconds();
	pulse_nixies(PULSE_DELAY, (int)(seconds / 10), seconds % 10);

	return;
}

void enable_nixies(void)
{
	PORTD |= 0b01000000;
	return;
}

void disable_nixies(void)
{
	PORTD &= 0b00011111; 
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
		display_digit(leftDigit);
		PORTD = (PORTD & 0b01111111) | 0b00100000;
		_delay_ms(10);

		// display right nixie
		display_digit(rightDigit);
		PORTD = (PORTD & 0b11011111) | 0b10000000;
		_delay_ms(10);
	}

	disable_nixies();
	return;
}

void set_time(void)
{
	DebouncingData debounceDataBottom;
	DebouncingData debounceDataTop;

	PORTD &= 0b01011111;
	enable_nixies();
	

	int hours = get_hours();
	int minutes = get_minutes();
	int seconds = get_seconds();

	// set hours
	const int MAX_HOURS = 24;
	while ((PIND & 0x08) != 0 || (PIND & 0x04) != 0)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) == 0)
		{
			if (hours - 1 >= 0) 
			{
				hours--; 
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) == 0)
		{
			if (hours + 1 < MAX_HOURS)
			{
				hours++;
			}
		}
		pulse_nixies(10, (int)(hours / 10), hours % 10);
	}

	while ((PIND & 0x08) == 0 || (PIND & 0x04) == 0)
	{
		continue;
	}

	// set minutes
	while ((PIND & 0x08) != 0 || (PIND & 0x04) != 0)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) == 0)
		{
			if (minutes - 1 >= 0)
			{
				minutes--;
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) == 0)
		{
			if (minutes + 1 < 60)
			{
				minutes++;
			}
		}
		pulse_nixies(10, (int)(minutes / 10), minutes % 10);
	}

	while ((PIND & 0x08) == 0 || (PIND & 0x04) == 0)
	{
		continue;
	}

	// set seconds
	while ((PIND & 0x08) != 0 || (PIND & 0x04) != 0)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) == 0)
		{
			if (seconds - 1 >= 0)
			{
				seconds--;
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) == 0)
		{
			if (seconds + 1 < 60)
			{
				seconds++;
			}	
		}

		pulse_nixies(10, (int)(seconds / 10), seconds % 10);
	}

	set_hours(hours);
	set_minutes(minutes);
	set_seconds(seconds);

	
	PORTD |= 0b10100000;
	enable_nixies();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			display_digit(j);
			_delay_ms(50);
		}
	}
	
	disable_nixies();

	_delay_ms(100);
	pulse_nixies(PULSE_DELAY, (int)(hours / 10), hours % 10);
	_delay_ms(100);
	pulse_nixies(PULSE_DELAY, (int)(minutes / 10), minutes % 10);
	_delay_ms(100);
	pulse_nixies(PULSE_DELAY, (int)(seconds / 10), seconds % 10);
	return;
}

void set_date(void)
{
	DebouncingData debounceDataTop;
	DebouncingData debounceDataBottom;

	PORTD &= 0b01011111;
	enable_nixies();

	int year = get_year();

	// set year
	while ((PIND & 0x08) != 0 || (PIND & 0x04) != 0)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) != 0)
		{
			if (year - 1 >= 0)
			{
				year--;
			}	
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) != 0)
		{
			if (year + 1 < 100)
			{
				year++;
			}
		}
		pulse_nixies(10, (int)(year / 10), year % 10);
	}

	bool leapYear = year % 4 == 0 ? true : false;

	while ((PIND & 0x08) == 0 || (PIND & 0x04) == 0)
	{
		continue;
	}
	
	int month = get_month();

	// set month
	while ((PIND & 0x08) != 0 || (PIND & 0x04) != 0)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) != 0)
		{
			if (month - 1 >= 0)
			{
				month--;
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) != 0)
		{
			if (month + 1 < 13)
			{
				month++;
			}	
		}
		pulse_nixies(10, (int)(month / 10), month % 10);
	}

	while ((PIND & 0x08) == 0 || (PIND & 0x04) == 0)
	{
		continue;
	}

	int day = get_day();
	const int MAX_DAYS = (month == 2 && leapYear) ? 29 : (month == 2) ? 28 : (month == 9 || month == 4 || month == 6 || month == 11) ? 30 : 31;

	// set day
	while ((PIND & 0x08) != 0 || (PIND & 0x04) != 0)
	{
		if (debounced_digital_read(&debounceDataBottom, BOTTOM_BUTTON) != 0)
		{
			if (day - 1 >= 0)
			{
				day--;
			}
		}

		if (debounced_digital_read(&debounceDataTop, TOP_BUTTON) != 0)
		{
			if (day + 1 <= MAX_DAYS)
			{
				day++;
			}
		}
		pulse_nixies(10, (int)(day / 10), day % 10);
	}

	set_day(day);
	set_month(month);
	set_year(year);

	PORTD |= 0b10100000;
	enable_nixies();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			display_digit(j);
			_delay_ms(50);
		}
	}
	
	disable_nixies();
	_delay_ms(100);

	pulse_nixies(PULSE_DELAY, (int)(day / 10), day % 10);
	_delay_ms(100);
	pulse_nixies(PULSE_DELAY, (int)(month / 10), month % 10);
	_delay_ms(100);
	pulse_nixies(PULSE_DELAY, (int)(year / 10), year % 10);
	return;
}