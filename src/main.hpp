#ifndef Main_Hpp
#define Main_Hpp

void blink(void);
void debug_blink(int pin, int count, int pulseDuration);
void top_button_press(void);
void bottom_button_press(void);
void top_blink(void);
void bottom_blink(void);
typedef struct DebouncingData_T
{
    int currentState = 0;
    int previousState = 0;
    long previousDebounceTime = 0;
} DebouncingData;
int debounced_digital_read(DebouncingData *buttonData, int pin);

#endif