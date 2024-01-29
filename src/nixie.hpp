#ifndef Nixie_Hpp
#define Nixie_Hpp

#include <Arduino.h>

class Nixie_Driver
{
    private:
        unsigned int A;
        unsigned int B;
        unsigned int C;
        unsigned int D;
        void write_0(void);
        void write_1(void);
        void write_2(void);
        void write_3(void);
        void write_4(void);
        void write_5(void);
        void write_6(void);
        void write_7(void);
        void write_8(void);
        void write_9(void);
    public:
        Nixie_Driver(unsigned int a, unsigned int b, unsigned int c, unsigned int d);
        void write_digit(unsigned int digit);
};

#endif