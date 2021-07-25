/**
 * @file time.hpp
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Time lib code
 * @version 0.1
 * @date 2021-07-27
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 * References:
 *         https://github.com/sidsingh78/EPOCH-to-time-date-converter/blob/master/epoch_conv.c
 *         https://www.oryx-embedded.com/doc/date__time_8c_source.html
 */
#include <dev/rtc/rtc.hpp>
#include <dev/graphics/tty.hpp>
#include <lib/stdio.hpp>
#include <lib/time.hpp>
namespace Time {

TimeDescriptor::TimeDescriptor()
{
    toDate();
}

TimeDescriptor::TimeDescriptor(int sec, int min, int hr, int d, int m, int y)
    : _seconds(sec)
    , _minutes(min)
    , _hour(hr)
    , _day(d)
    , _month(m)
    , _year(y)
{
    // Initialize nothing.
}
TimeDescriptor::~TimeDescriptor()
{
    // Nothing to deconstruct
}

void TimeDescriptor::converterEpochToDate(uint64_t epoch)
{
    int a, b, c, day, month, year;
    setSeconds(epoch % 60);
    epoch /= 60;
    setMinutes(epoch % 60);
    epoch /= 60;
    setHour(epoch % 24);
    epoch /= 24;

    // This is the formula used by everyone. No idea how it works :p
    a = ((4 * epoch + 102032) / 146097 + 15);
    b = (epoch + 2442113 + a - (a / 4));
    year = (20 * b - 2442) / 7305;
    c = b - 365 * year - (year / 4);
    month = c * 1000 / 30601;
    day = c - month * 30 - month * 601 / 1000;
    if (month <= 13) {
        year -= 4716;
        month -= 1;
    } else {
        year -= 4715;
        month -= 13;
    }

    setDay(day);
    setMonth(month);
    setYear(year);
}

void TimeDescriptor::printDate()
{
    kprintf(
        DBG_INFO
        "UTC: %i/%i/%i %i:%i\n",
        getMonth(),
        getDay(),
        getYear(),
        getHour(),
        getMinutes());
}

void TimeDescriptor::toDate()
{
    uint64_t epoch = RTC::getEpoch();
    converterEpochToDate(epoch);
    // TODO: for another GMT, add or subtract time zone in epoch number
    // e.g
    // for GMT -3 =(-3*3600)= -10800
    // converterEpochToDate(epoch-10800);
    converterEpochToDate(epoch);
}

} // !namespace Time
