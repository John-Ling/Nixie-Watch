#ifndef Utils_Hpp
#define Utils_Hpp
#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct DebouncingData_T
{
    int currentState = 0;
    int previousState = 0;
    unsigned long previousDebounceTime = 0;
} DebouncingData;

int pin_to_port(int pin);
void init_millis(void);
ISR(TIMER1_COMPA_vect);
unsigned long millis(void);
void display_digit(int digit);
int debounced_digital_read(DebouncingData *buttonData, int pin);

#endif