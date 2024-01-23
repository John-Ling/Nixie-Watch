#ifndef Main_Hpp
#define Main_Hpp

void blink(void);
void pulse_nixies(unsigned long milliseconds, int leftDigit, int rightDigit);
void handle_bottom_button_press(void);
void handle_top_button_press(void);
void handle_tilt(void);
void top_button_press(void);
void bottom_button_press(void);
typedef struct DebouncingData_T
{
    int currentState = 0;
    int previousState = 0;
    long previousDebounceTime = 0;
} DebouncingData;
int debounced_digital_read(DebouncingData *buttonData, int pin);

#endif