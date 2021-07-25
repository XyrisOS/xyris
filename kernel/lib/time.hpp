/**
 * @file time.hpp
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Time lib code 
 * @version 0.1
 * @date 2021-07-27
 *
 * @copyright Copyright the Panix Contributors (c) 2021
 *
 * References:
 *         https://github.com/sidsingh78/EPOCH-to-time-date-converter/blob/master/epoch_conv.c
 *         https://www.oryx-embedded.com/doc/date__time_8c_source.html
 */
#pragma once

namespace Time {

class TimeDescriptor {
public:
    // Constructors
    TimeDescriptor(int sec, int min, int hr, int d, int m, int y);
    TimeDescriptor();
    ~TimeDescriptor();
    // Getters
    int getSeconds() { return _seconds; }
    int getMinutes() { return _minutes; }
    int getHour() { return _hour; }
    int getDay() { return _day; }
    int getMonth() { return _month; }
    int getYear() { return _year; }
    // Setters
    void setSeconds(int sec) { _seconds = sec; }
    void setMinutes(int min) { _minutes = min; }
    void setHour(int hr) { _hour = hr; }
    void setDay(int d) { _day = d; }
    void setMonth(int m) { _month = m; }
    void setYear(int y) { _year = y; }
    // methods
    void toDate();
    void printDate();

private:
    void converterEpochToDate(uint64_t Number);
    int _seconds;
    int _minutes;
    int _hour;
    int _day;
    int _month;
    int _year;
    int _dayOfWeek;
    //int _dayOfYear;
    //int _dayLight;
};

};
