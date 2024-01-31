#ifndef Nixie_Hpp
#define Nixie_Hpp

#include <Arduino.h>

class Nixie_Driver
{
private:
    // inputs for bcd encoder
    int A;
    int B;
    int C;
    int D;
    void display_0(void);
    void display_1(void);
    void display_2(void);
    void display_3(void);
    void display_4(void);
    void display_5(void);
    void display_6(void);
    void display_7(void);
    void display_8(void);
    void display_9(void);

public:
    Nixie_Driver(int a, int b, int c, int d);
    void display_digit(int digit);
};

#endif