#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timelib.h"

#define SECS_IN_MIN 60
#define SECS_IN_HOUR 3600  // 60 * 60
#define MINS_IN_HOUR 60

typedef struct {
    TDateTimeTZ datetimetz;
    int num_hours;
} TInterval;

int cmp_interval(const void *a, const void *b) {
    TInterval *i_a = (TInterval *)a;
    TInterval *i_b = (TInterval *)b;
    if (convertDateTimeTZToUnixTimestamp(i_a->datetimetz) < convertDateTimeTZToUnixTimestamp(i_b->datetimetz))
        return -1;
    if (convertDateTimeTZToUnixTimestamp(i_a->datetimetz) > convertDateTimeTZToUnixTimestamp(i_b->datetimetz))
        return 1;
    if (i_a->num_hours <= i_b->num_hours)
        return -1;
    return 1;
}

typedef struct {
    char name[BUF_SIZE];
    int timezones_index;
    int K;
    TInterval *intervals;
} TPerson;

int cmp_person(const void *a, const void *b) {
    TPerson *p_a = (TPerson *)a;
    TPerson *p_b = (TPerson *)b;
    return strcmp(p_a->name, p_b->name);
}

int main() {
    int T = 0; scanf("%d", &T);
    TTimezone *timezones = (TTimezone *)malloc(T * sizeof(TTimezone));
    for (int i = 0; i < T; i++) {
        scanf("%s", timezones[i].name);
        scanf("%hhd", &timezones[i].utc_hour_difference);
    }
    int P = 0; scanf("%d", &P);
    TPerson *persons = (TPerson *)malloc(P * sizeof(TPerson));
    for (int i = 0; i < P; i++) {
        scanf("%s", persons[i].name);
        char nameTZ[BUF_SIZE]; scanf("%s", nameTZ);
        for (int t = 0; t < T; t++)
            if (!strcmp(timezones[t].name, nameTZ)) {
                persons[i].timezones_index = t;
                break;
            }
        scanf("%d", &persons[i].K);
        persons[i].intervals = (TInterval *)malloc(persons[i].K * sizeof(TInterval));
        for (int j = 0; j < persons[i].K; j++) {
            persons[i].intervals[j].datetimetz.tz = &timezones[persons[i].timezones_index];
            scanf("%d", &persons[i].intervals[j].datetimetz.date.year);
            scanf("%hhd", &persons[i].intervals[j].datetimetz.date.month);
            scanf("%hhd", &persons[i].intervals[j].datetimetz.date.day);
            scanf("%hhd", &persons[i].intervals[j].datetimetz.time.hour);
            persons[i].intervals[j].datetimetz.time.min = 0;
            persons[i].intervals[j].datetimetz.time.sec = 0;
            scanf("%d", &persons[i].intervals[j].num_hours);
        }
        qsort(persons[i].intervals, persons[i].K, sizeof(TInterval), cmp_interval);
    }
    for (int i = 0; i < P; i++)
        for (int j = 0; j < persons[i].K - 1; j++) {
            unsigned int timestamp_start1 = convertDateTimeTZToUnixTimestamp(persons[i].intervals[j].datetimetz);
            unsigned int timestamp_finish1 = timestamp_start1 + persons[i].intervals[j].num_hours * SECS_IN_HOUR;
            unsigned int timestamp_start2 = convertDateTimeTZToUnixTimestamp(persons[i].intervals[j + 1].datetimetz);
            unsigned int timestamp_finish2 = timestamp_start2 + persons[i].intervals[j + 1].num_hours * SECS_IN_HOUR;
            if (timestamp_finish1 == timestamp_start2) {
                persons[i].intervals[j].num_hours = (int)((timestamp_finish2 - timestamp_start1) / SECS_IN_HOUR);
                for (int k = j + 1; k < persons[i].K - 1; k++)
                    persons[i].intervals[k] = persons[i].intervals[k + 1];
                persons[i].K--;
                persons[i].intervals = (TInterval *)realloc(persons[i].intervals, persons[i].K * sizeof(TInterval));
                j--;
            }
        }
    int F = 0; scanf("%d", &F);
    int num_hours_event = 0; scanf("%d", &num_hours_event);
    int possible = 0;
    unsigned int timestamp_event_start = 0;
    int num_intervals = 0;
    TInterval *intervals = NULL;
    for (int i = 0; i < P; i++)
        for (int j = 0; j < persons[i].K; j++) {
            num_intervals++;
            intervals = (TInterval *)realloc(intervals, num_intervals * sizeof(TInterval));
            intervals[num_intervals - 1] = persons[i].intervals[j];
        }
    qsort(intervals, num_intervals, sizeof(TInterval), cmp_interval);
    for (int step = 0; step < num_intervals; step++) {
        int num_persons_event = 0;
        timestamp_event_start = convertDateTimeTZToUnixTimestamp(intervals[step].datetimetz);
        unsigned int timestamp_event_finish = timestamp_event_start + num_hours_event * SECS_IN_HOUR;
        for (int i = 0; i < P; i++)
            for (int j = 0; j < persons[i].K; j++) {
                unsigned int timestamp_start = convertDateTimeTZToUnixTimestamp(persons[i].intervals[j].datetimetz);
                unsigned int timestamp_finish = timestamp_start + persons[i].intervals[j].num_hours * SECS_IN_HOUR;
                if (timestamp_start <= timestamp_event_start && timestamp_finish >= timestamp_event_finish) {
                    num_persons_event++;
                    break;
                }
            }
        if (num_persons_event >= F) {
            possible = 1;
            break;
        }
    }
    qsort(persons, P, sizeof(TPerson), cmp_person);
    if (possible) {
        unsigned int timestamp_event_finish = timestamp_event_start + num_hours_event * SECS_IN_HOUR;
        for (int i = 0; i < P; i++) {
            int valid = 0;
            printf("%s: ", persons[i].name);
            for (int j = 0; j < persons[i].K; j++) {
                unsigned int timestamp_start = convertDateTimeTZToUnixTimestamp(persons[i].intervals[j].datetimetz);
                unsigned int timestamp_finish = timestamp_start + persons[i].intervals[j].num_hours * SECS_IN_HOUR;
                if (timestamp_start <= timestamp_event_start && timestamp_finish >= timestamp_event_finish) {
                    valid = 1;
                    break;
                }
            }
            if (valid) {
                TDateTimeTZ aux;
                aux = convertUnixTimestampToDateTimeTZ(timestamp_event_start, timezones, persons[i].timezones_index);
                printDateTimeTZ(aux);
            } else {
                printf("invalid\n");
            }
        }
    } else {
        printf("imposibil\n");
    }
    free(intervals);
    for (int i = 0; i < P; i++)
        free(persons[i].intervals);
    free(persons);
    free(timezones);
    return 0;
}
