#include "nixie.hpp"

Nixie_Driver::Nixie_Driver(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
    this->A = a;
    this->B = b;
    this->C = c;
    this->D = d;
    return;
}

void Nixie_Driver::write_digit(unsigned int digit)
{
    if (digit > 9)
    {
        return;
    }

    switch (digit)
    {
    case 0:
        this->write_0();
    case 1:
        this->write_1();
    case 2:
        this->write_2();
    case 3:
        this->write_3();
    case 4:
        this->write_4();
    case 5:
        this->write_5();
    case 6:
        this->write_6();
    case 7:
        this->write_7();
    case 8:
        this->write_8();
    case 9:
        this->write_9();
    }

    return;
}

void Nixie_Driver::write_0(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::write_1(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::write_2(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, HIGH);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::write_3(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, HIGH);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::write_4(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, HIGH);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::write_5(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, HIGH);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::write_6(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, HIGH);
    digitalWrite(this->C, HIGH);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::write_7(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, HIGH);
    digitalWrite(this->C, HIGH);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::write_8(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, HIGH);
    return;
}

void Nixie_Driver::write_9(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, HIGH);
    return;
}