#include "nixie.hpp"

Nixie_Driver::Nixie_Driver(int a, int b, int c, int d)
{
    this->A = a;
    this->B = b;
    this->C = c;
    this->D = d;

    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);

    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
    digitalWrite(D, LOW);
    return;
}

void Nixie_Driver::write_digit(int digit)
{
    if (digit > 9 || digit < 0)
    {
        return;
    }

    switch (digit)
    {
    case 0:
        this->write_0();
        break;
    case 1:
        this->write_1();
        break;
    case 2:
        this->write_2();
        break;
    case 3:
        this->write_3();
        break;
    case 4:
        this->write_4();
        break;
    case 5:
        this->write_5();
        break;
    case 6:
        this->write_6();
        break;
    case 7:
        this->write_7();
        break;
    case 8:
        this->write_8();
        break;
    case 9:
        this->write_9();
        break;
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