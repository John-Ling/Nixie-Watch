#ifndef Utils_Hpp
#define Utils_Hpp

typedef struct DebouncingData_T
{
    int currentState = 0;
    int previousState = 0;
    unsigned long previousDebounceTime = 0;
} DebouncingData;

int pin_to_port(int pin);
void display_digit(int digit);
int debounced_digital_read(DebouncingData *buttonData, int pin);

#endif