#include <stdio.h>
#include <stdlib.h>
#include "timelib.h"

#define UTC_START_DAY 1
#define UTC_START_MONTH 1
#define UTC_START_YEAR 1970

#define SECS_IN_MIN 60
#define SECS_IN_HOUR 3600  // 60 * 60
#define MINS_IN_HOUR 60
#define SECS_IN_DAY 86400  // 60 * 60 * 24
#define HOURS_IN_DAY 24
#define LEAP_YEAR(year) (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
#define SECS_IN_COMMON_YEAR 31536000  // 60 * 60 * 24 * 365
#define DAYS_IN_COMMON_YEAR 365
#define SECS_IN_LEAP_YEAR 31622400  // 60 * 60 * 24 * 366
#define DAYS_IN_LEAP_YEAR 366
#define MONTHS_IN_YEAR 12
const unsigned int DAYS_IN_MONTHS_COMMON_YEAR[MONTHS_IN_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const unsigned int DAYS_IN_MONTHS_LEAP_YEAR[MONTHS_IN_YEAR] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

TTime convertUnixTimestampToTime(unsigned int timestamp) {
    TTime result = {0, 0, 0};
    result.hour = (timestamp % SECS_IN_DAY) / SECS_IN_HOUR;
    result.min = (timestamp % SECS_IN_HOUR) / SECS_IN_MIN;
    result.sec = timestamp % SECS_IN_MIN;
    return result;
}

TDate convertUnixTimestampToDateWithoutLeapYears(unsigned int timestamp) {
    TDate result = {UTC_START_DAY, UTC_START_MONTH, UTC_START_YEAR};
    result.year += timestamp / SECS_IN_COMMON_YEAR;
    unsigned int day = (timestamp % SECS_IN_COMMON_YEAR) / SECS_IN_DAY;
    for (int idx_mnth = 0; idx_mnth < MONTHS_IN_YEAR; idx_mnth++)
        if (day >= DAYS_IN_MONTHS_COMMON_YEAR[idx_mnth]) {
            day -= DAYS_IN_MONTHS_COMMON_YEAR[idx_mnth];
            result.month++;
        } else {
            break;
        }
    result.day += (unsigned char)day;
    return result;
}

TDate convertUnixTimestampToDate(unsigned int timestamp) {
    TDate result = {UTC_START_DAY, UTC_START_MONTH, UTC_START_YEAR};
    unsigned int timestamp_copy = timestamp;
    while (1) {
        if (LEAP_YEAR(result.year)) {
            if (timestamp_copy >= SECS_IN_LEAP_YEAR) {
                timestamp_copy -= SECS_IN_LEAP_YEAR;
                result.year++;
            } else {
                break;
            }
        } else {
            if (timestamp_copy >= SECS_IN_COMMON_YEAR) {
                timestamp_copy -= SECS_IN_COMMON_YEAR;
                result.year++;
            } else {
                break;
            }
        }
    }
    unsigned int day = timestamp_copy / SECS_IN_DAY;
    if (LEAP_YEAR(result.year)) {
        for (int idx_mnth = 0; idx_mnth < MONTHS_IN_YEAR; idx_mnth++)
            if (day >= DAYS_IN_MONTHS_LEAP_YEAR[idx_mnth]) {
                day -= DAYS_IN_MONTHS_LEAP_YEAR[idx_mnth];
                result.month++;
            } else {
                break;
            }
    } else {
        for (int idx_mnth = 0; idx_mnth < MONTHS_IN_YEAR; idx_mnth++)
            if (day >= DAYS_IN_MONTHS_COMMON_YEAR[idx_mnth]) {
                day -= DAYS_IN_MONTHS_COMMON_YEAR[idx_mnth];
                result.month++;
            } else {
                break;
            }
    }
    result.day += (unsigned char)day;
    return result;
}

TDateTimeTZ convertUnixTimestampToDateTimeTZ(unsigned int timestamp, TTimezone *timezones, int timezone_index) {
    TDateTimeTZ result = {{0, 0, 0}, {0, 0, 0}, NULL};
    result.date = convertUnixTimestampToDate(timestamp);
    result.time = convertUnixTimestampToTime(timestamp);
    result.tz = &timezones[timezone_index];
    if (result.tz->utc_hour_difference < 0) {
        if (result.time.hour + result.tz->utc_hour_difference >= 0) {
            result.time.hour += result.tz->utc_hour_difference;
        } else {
            result.time.hour += HOURS_IN_DAY;
            result.time.hour += result.tz->utc_hour_difference;
            if (result.date.day > 1) {
                result.date.day--;
            } else if (result.date.month > 1) {
                if (LEAP_YEAR(result.date.year))
                    result.date.day = DAYS_IN_MONTHS_LEAP_YEAR[(result.date.month - 1) - 1];
                else
                    result.date.day = DAYS_IN_MONTHS_COMMON_YEAR[(result.date.month - 1) - 1];
                result.date.month--;
            } else {
                result.date.day = DAYS_IN_MONTHS_COMMON_YEAR[MONTHS_IN_YEAR - 1];
                result.date.month = MONTHS_IN_YEAR;
                result.date.year--;
            }
        }
    }
    if (result.tz->utc_hour_difference > 0) {
        if (result.time.hour + result.tz->utc_hour_difference < HOURS_IN_DAY) {
            result.time.hour += result.tz->utc_hour_difference;
        } else {
            result.time.hour += result.tz->utc_hour_difference;
            result.time.hour -= HOURS_IN_DAY;
            if (LEAP_YEAR(result.date.year)) {
                if (result.date.day < DAYS_IN_MONTHS_LEAP_YEAR[result.date.month - 1]) {
                    result.date.day++;
                } else if (result.date.month <= MONTHS_IN_YEAR) {
                    result.date.day = 1;
                    result.date.month++;
                } else {
                    result.date.day = 1;
                    result.date.month = 1;
                    result.date.year++;
                }
            } else {
                if (result.date.day < DAYS_IN_MONTHS_COMMON_YEAR[result.date.month - 1]) {
                    result.date.day++;
                } else if (result.date.month <= MONTHS_IN_YEAR) {
                    result.date.day = 1;
                    result.date.month++;
                } else {
                    result.date.day = 1;
                    result.date.month = 1;
                    result.date.year++;
                }
            }
        }
    }
    return result;
}

unsigned int convertDateTimeTZToUnixTimestamp(TDateTimeTZ datetimetz) {
    unsigned int timestamp = 0;
    TDate UTC_START = {UTC_START_DAY, UTC_START_MONTH, UTC_START_YEAR};
    while (UTC_START.year < datetimetz.date.year) {
        if (LEAP_YEAR(UTC_START.year))
            timestamp += SECS_IN_LEAP_YEAR;
        else
            timestamp += SECS_IN_COMMON_YEAR;
        UTC_START.year++;
    }
    while (UTC_START.month < datetimetz.date.month) {
        if (LEAP_YEAR(datetimetz.date.year))
            timestamp += DAYS_IN_MONTHS_LEAP_YEAR[UTC_START.month - 1] * SECS_IN_DAY;
        else
            timestamp += DAYS_IN_MONTHS_COMMON_YEAR[UTC_START.month - 1] * SECS_IN_DAY;
        UTC_START.month++;
    }
    timestamp += (datetimetz.date.day - UTC_START.day) * SECS_IN_DAY;
    timestamp += datetimetz.time.hour * SECS_IN_HOUR;
    timestamp -= datetimetz.tz->utc_hour_difference * SECS_IN_HOUR;
    timestamp += datetimetz.time.min * SECS_IN_MIN;
    timestamp += datetimetz.time.sec;
    return timestamp;
}

void printDateTimeTZ(TDateTimeTZ datetimetz) {
    const unsigned char MONTHS_NAMES[12][11] = {"ianuarie", "februarie", "martie", "aprilie", "mai", "iunie", \
                                                "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};
    printf("%02d %s %d, ", datetimetz.date.day, MONTHS_NAMES[datetimetz.date.month - 1], datetimetz.date.year);
    printf("%02d:%02d:%02d ", datetimetz.time.hour, datetimetz.time.min, datetimetz.time.sec);
    printf("%s (UTC%+d)\n", datetimetz.tz->name, datetimetz.tz->utc_hour_difference);
}
