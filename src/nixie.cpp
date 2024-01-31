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

void Nixie_Driver::display_digit(int digit)
{
    if (digit > 9 || digit < 0)
    {
        return;
    }

    switch (digit)
    {
    case 0:
        this->display_0();
        break;
    case 1:
        this->display_1();
        break;
    case 2:
        this->display_2();
        break;
    case 3:
        this->display_3();
        break;
    case 4:
        this->display_4();
        break;
    case 5:
        this->display_5();
        break;
    case 6:
        this->display_6();
        break;
    case 7:
        this->display_7();
        break;
    case 8:
        this->display_8();
        break;
    case 9:
        this->display_9();
        break;
    }

    return;
}

void Nixie_Driver::display_0(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::display_1(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::display_2(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, HIGH);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::display_3(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, HIGH);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::display_4(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, HIGH);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::display_5(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, HIGH);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::display_6(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, HIGH);
    digitalWrite(this->C, HIGH);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::display_7(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, HIGH);
    digitalWrite(this->C, HIGH);
    digitalWrite(this->D, LOW);
    return;
}

void Nixie_Driver::display_8(void)
{
    digitalWrite(this->A, LOW);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, HIGH);
    return;
}

void Nixie_Driver::display_9(void)
{
    digitalWrite(this->A, HIGH);
    digitalWrite(this->B, LOW);
    digitalWrite(this->C, LOW);
    digitalWrite(this->D, HIGH);
    return;
}