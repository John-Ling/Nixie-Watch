#include "rtc.hpp"

int get_seconds(void)
{
    prepare_read(0x81);
    int seconds = bcd_to_denary(read_data() & 0x7F);
    PORTB &= ~(1 << 1);
    return seconds;
}

int get_minutes(void)
{
    prepare_read(0x83);
    int minutes = bcd_to_denary(read_data() & 0x7F);
    PORTB &= ~(1 << 1);
    return minutes;
}

int get_hours(void)
{
    prepare_read(0x85);
    int hours = bcd_to_denary(read_data() & 0x3F);
    PORTB &= ~(1 << 1);
    return hours;
}

int get_day(void)
{
    prepare_read(0x87);
    int day = bcd_to_denary(read_data() & 0x3F);
    PORTB &= ~(1 << 1);
    return day;
}

int get_month(void)
{
    prepare_read(0x89);
    int month = bcd_to_denary(read_data() & 0x1F);
    PORTB &= ~(1 << 1);
    return month;
}

int get_year(void)
{
    prepare_read(0x8D);
    int year = bcd_to_denary(read_data() & 0xFF);
    PORTB &= ~(1 << 1);
    return year;
}

void set_seconds(int seconds)
{
    prepare_write(0x80);
    write_data(denary_to_bcd(seconds % 60));
    PORTB &= ~(1 << 1);
    return;
}

void set_minutes(int minutes)
{
    prepare_write(0x82);
    write_data(denary_to_bcd(minutes % 60));
    PORTB &= ~(1 << 1);
    return;
}

void set_hours(int hours)
{
    prepare_write(0x84);
    write_data(denary_to_bcd(hours % 24)); // come back to this if want to support 12 and 24 hour mode
    PORTB &= ~(1 << 1);
    return;
}

void set_day(int day)
{
    prepare_write(0x86);
    write_data(denary_to_bcd(day % 32));
    PORTB &= ~(1 << 1);
    return;
}

void set_month(int month)
{
    prepare_write(0x88);
    write_data(denary_to_bcd(month % 13));
    PORTB &= ~(1 << 1);
    return;
}

void set_year(int year)
{
    prepare_write(0x8C);
    write_data(denary_to_bcd(year % 100));
    PORTB &= ~(1 << 1);
    return;
}

void prepare_read(uint8_t address)
{
    DDRB |= (1 << 2);
    PORTB |= (1 << 1);
    uint8_t commandByte = 0x81 | address;
    write_data(commandByte);
    DDRB &= ~(1 << 2);
    return;
}

void prepare_write(uint8_t address)
{
    DDRB |= (1 << 2);
    PORTB |= (1 << 1);
    uint8_t commandByte = 0x80 | address;
    write_data(commandByte);
    return;
}

uint8_t read_data(void)
{
    uint8_t received = 0;
    for (int i = 0; i < 8; i++)
    {
        received |= ((PINB & (1 << 2)) != 0) << i;
        pulse_clock();
    }

    return received;
}

void write_data(uint8_t address)
{
    for (int i = 0; i < 8; i++) {
        if (address & (1 << i))
        {
            PORTB |= (1 << 2);
        }
        else
        {
            PORTB &= ~(1 << 2);
        }
        pulse_clock();
    }
    return;
}

void pulse_clock(void)
{
    PORTB |= (1 << 3);
    _delay_us(10);
    PORTB &= ~(1 << 3);
    return;
}

int bcd_to_denary(uint8_t bcd)
{
    return (bcd / 16 * 10) + (bcd % 16);
}

uint8_t denary_to_bcd(int denary)
{
    return (denary / 10 * 16) + (denary % 10);
}
