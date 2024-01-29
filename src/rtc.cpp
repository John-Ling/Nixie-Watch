#include <Arduino.h>
#include "rtc.hpp"

RTC::RTC(int ce, int io, int sclk)
{
    this->CE = ce;
    this->IO = io;
    this->SCLK = sclk;

    pinMode(CE, OUTPUT);
    pinMode(SCLK, OUTPUT);
    pinMode(IO, INPUT);

    digitalWrite(CE, LOW);
    digitalWrite(SCLK, LOW);
    return;
};

int RTC::get_seconds(void)
{
    this->prepare_read(0x81);
    int seconds = this->bcd_to_denary(this->read_data() & 0x7F);
    digitalWrite(CE, LOW);
    return seconds;
}

int RTC::get_minutes(void)
{
    this->prepare_read(0x83);
    int minutes = this->bcd_to_denary(this->read_data() & 0x7F);
    digitalWrite(CE, LOW);
    return minutes;
}

int RTC::get_hours(void)
{
    this->prepare_read(0x85);
    int hours = this->bcd_to_denary(this->read_data() & 0x3F);
    digitalWrite(CE, LOW);
    return hours;
}

int RTC::get_day(void)
{
    this->prepare_read(0x87);
    int day = this->bcd_to_denary(this->read_data() & 0x3F);
    digitalWrite(CE, LOW);
    return day;
}

int RTC::get_month(void)
{
    this->prepare_read(0x89);
    int month = this->bcd_to_denary(this->read_data() & 0x1F);
    digitalWrite(CE, LOW);
    return month;
}

int RTC::get_year(void)
{
    this->prepare_read(0x8D);
    int year = this->bcd_to_denary(this->read_data() & 0xFF);
    digitalWrite(CE, LOW);
    return year;
}

void RTC::set_seconds(int seconds)
{
    this->prepare_write(0x80);
    this->write_data(this->denary_to_bcd(seconds % 60));
    digitalWrite(CE, LOW);
    return;
}

void RTC::set_minutes(int minutes)
{
    this->prepare_write(0x82);
    this->write_data(this->denary_to_bcd(minutes % 60));
    digitalWrite(CE, LOW);
    return;
}

void RTC::set_hours(int hours)
{
    this->prepare_write(0x84);
    this->write_data(this->denary_to_bcd(hours % 24)); // come back to this if want to support 12 and 24 hour mode
    digitalWrite(CE, LOW);
    return;
}

void RTC::set_day(int day)
{
    this->prepare_write(0x86);
    this->write_data(this->denary_to_bcd(day % 32));
    digitalWrite(CE, LOW);
    return;
}

void RTC::set_month(int month)
{
    this->prepare_write(0x88);
    this->write_data(this->denary_to_bcd(month % 13));
    digitalWrite(CE, LOW);
    return;
}

void RTC::set_year(int year)
{
    this->prepare_write(0x8C);
    this->write_data(this->denary_to_bcd(year % 100));
    digitalWrite(CE, LOW);
    return;
}

void RTC::prepare_read(uint8_t address)
{
    pinMode(this->IO, OUTPUT);
    digitalWrite(this->CE, HIGH);
    uint8_t commandByte = 0x81 | address;
    this->write_data(commandByte);
    pinMode(this->IO, INPUT);
    return;
}

void RTC::prepare_write(uint8_t address)
{
    pinMode(this->IO, OUTPUT);
    digitalWrite(this->CE, HIGH);
    uint8_t commandByte = 0x80 | address;
    this->write_data(commandByte);
    return;
}

uint8_t RTC::read_data(void)
{
    uint8_t received = 0;
    for (int i = 0; i < 8; i++)
    {
        received |= digitalRead(this->IO) << i;
        this->pulse_clock();
    }

    return received;
}

void RTC::write_data(uint8_t address)
{
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(this->IO, address & 0x01);
        this->pulse_clock();
        address >>= 1;
    }
    return;
}

void RTC::pulse_clock(void)
{
    digitalWrite(this->SCLK, HIGH);
    digitalWrite(this->SCLK, LOW);
    return;
}

int RTC::bcd_to_denary(uint8_t bcd)
{
    return (bcd / 16 * 10) + (bcd % 16);
}

uint8_t RTC::denary_to_bcd(int denary)
{
    return (denary / 10 * 16) + (denary % 10);
}
