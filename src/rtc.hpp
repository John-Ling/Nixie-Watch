#include <stdint.h>
#ifndef Rtc_Hpp
#define Rtc_Hpp

typedef struct RTCData_T
{
    int seconds = -1;
    int minutes = -1;
    int hours = -1;
    int day = -1;
    int month = -1;
    int year = -1;
} RTCData;

class RTC
{
    private:
        int CE;
        int IO;
        int SCLK;
        RTCData data;
        void prepare_read(uint8_t address);
        void prepare_write(uint8_t address);
        uint8_t read_data(void);
        void write_data(uint8_t byte);
        void pulse_clock(void);
        int bcd_to_denary(uint8_t bcd);
        uint8_t denary_to_bcd(int denary);
    public:
        RTC(int ce, int io, int sclk);
        int get_seconds(void);
        int get_minutes(void);
        int get_hours(void);
        int get_day(void);
        int get_month(void);
        int get_year(void);
        void set_seconds(unsigned int seconds);
        void set_minutes(unsigned int minutes);
        void set_hours(unsigned int hours);
        void set_day(unsigned int day);
        void set_month(unsigned int month);
        void set_year(unsigned int year);
};

#endif