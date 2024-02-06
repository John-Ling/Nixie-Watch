#ifndef Helpers_Hpp
#define Helpers_Hpp

int pin_to_port(int pin);
typedef struct DebouncingData_T
{
    int currentState = 0;
    int previousState = 0;
    unsigned long previousDebounceTime = 0;
} DebouncingData;
int debounced_digital_read(DebouncingData *buttonData, int pin);

#endif