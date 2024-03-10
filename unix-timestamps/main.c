#include <stdio.h>
#include <stdlib.h>
#include "timelib.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: ./check <task_number>\n");
        return 1;
    }

    int task = atoi(argv[1]);
    
    unsigned int timestamp;
    TTime time;
    TDate date;
    TDateTimeTZ datetimetz;

    TTimezone *timezones = malloc(sizeof(TTimezone) * 50);
    int nr_timezones;
    int tz_index;

    TTimezone timezone;

    switch (task) {
        case 1:
            scanf("%u", &timestamp);
            time = convertUnixTimestampToTime(timestamp);
            printf("%hhd %hhd %hhd\n", time.hour, time.min, time.sec);
            break;
        case 2:
            scanf("%u", &timestamp);
            date = convertUnixTimestampToDateWithoutLeapYears(timestamp);
            printf("%hhd %hhd %u\n", date.day, date.month, date.year);
            break;
        case 3:
            scanf("%u", &timestamp);
            date = convertUnixTimestampToDate(timestamp);
            printf("%hhd %d %u\n", date.day, date.month, date.year);
            break;
        case 4:
            scanf("%u", &timestamp);
            scanf("%d", &nr_timezones);
            for (int i=0; i<nr_timezones; i++) {
                scanf("%s", timezones[i].name);
                scanf("%hhd", &(timezones[i].utc_hour_difference));
            }
            scanf("%d", &tz_index);
            datetimetz = convertUnixTimestampToDateTimeTZ(timestamp, timezones, tz_index);
            printf("%hhd %hhd %u %hhd %hhd %hhd\n", datetimetz.date.day, datetimetz.date.month, datetimetz.date.year, datetimetz.time.hour, datetimetz.time.min, datetimetz.time.sec);
            break;
        case 5:
            scanf("%hhd %hhd %u %hhd %hhd %hhd", &(datetimetz.date.day), &(datetimetz.date.month), &(datetimetz.date.year), &(datetimetz.time.hour), &(datetimetz.time.min), &(datetimetz.time.sec));
            scanf("%d", &nr_timezones);
            for (int i=0; i<nr_timezones; i++) {
                scanf("%s", timezones[i].name);
                scanf("%hhd", &(timezones[i].utc_hour_difference));
            }
            scanf("%d", &tz_index);
            datetimetz.tz = timezones+tz_index;
            timestamp = convertDateTimeTZToUnixTimestamp(datetimetz);
            printf("%u\n", timestamp);
            break;
        case 6:
            scanf("%hhd %hhd %u %hhd %hhd %hhd", &(datetimetz.date.day), &(datetimetz.date.month), &(datetimetz.date.year), &(datetimetz.time.hour), &(datetimetz.time.min), &(datetimetz.time.sec));
            scanf("%s %hhd", timezone.name, &(timezone.utc_hour_difference));
            datetimetz.tz = &timezone;
            printDateTimeTZ(datetimetz);
            break;
        default:
            printf("Invalid task number.\n");
            return 1;
    }

    free(timezones);

    return 0;
}
