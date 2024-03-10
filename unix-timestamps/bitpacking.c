#include <stdio.h>
#include <stdlib.h>
#include "timelib.h"

#define TASK7 7
#define TASK8 8

#define UTC_START_DAY 1
#define UTC_START_MONTH 1
#define UTC_START_YEAR 1970

#define UINT_SIZE 32
#define DAY_SIZE 5
#define DAY_MASK 31
#define MONTH_SIZE 4
#define MONTH_MASK 15
#define YEAR_SIZE 6
#define YEAR_MASK 63
#define DATE_SIZE 15

int cmp_date(const void *a, const void *b) {
    TDate *data_a = (TDate *)a;
    TDate *data_b = (TDate *)b;
    if (data_a->year < data_b->year)
        return -1;
    if (data_a->year > data_b->year)
        return 1;
    if (data_a->month < data_b->month)
        return -1;
    if (data_a->month > data_b->month)
        return 1;
    if (data_a->day < data_b->day)
        return -1;
    return 1;
}

int main() {
    const unsigned char MONTHS_NAMES[12][11] = {"ianuarie", "februarie", "martie", "aprilie", "mai", "iunie", \
                                                "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};
    int task = 0; scanf("%d", &task);
    if (task == TASK7) {
        unsigned int N = 0; scanf("%d", &N);
        TDate *dates = (TDate *)malloc(N * sizeof(TDate));
        unsigned int x = 0;
        for (int i = 0; i < N; i++) {
            scanf("%d", &x);
            dates[i].day = x & DAY_MASK;
            dates[i].month = (x >> DAY_SIZE) & MONTH_MASK;
            dates[i].year = (x >> (DAY_SIZE + MONTH_SIZE)) & YEAR_MASK;
        }
        qsort(dates, N, sizeof(TDate), cmp_date);
        for (int i = 0; i < N; i++)
            printf("%d %s %d\n", dates[i].day, MONTHS_NAMES[dates[i].month - 1], UTC_START_YEAR + dates[i].year);
        free(dates);
    }
    if (task == TASK8) {
        unsigned int num_dates = 0; scanf("%d", &num_dates);
        TDate *dates = (TDate *)malloc(num_dates * sizeof(TDate));
        unsigned int N = (num_dates * DATE_SIZE) / UINT_SIZE + ((num_dates * DATE_SIZE) % UINT_SIZE ? 1 : 0);
        unsigned int dates_uints[N];
        for (int i = 0; i < N; i++)
            scanf("%d", &dates_uints[i]);
        unsigned int M = N / UINT_SIZE + (N % UINT_SIZE ? 1 : 0);
        unsigned int BC_uints[M];
        for (int i = 0; i < M; i++)
            scanf("%d", &BC_uints[i]);
        int cnt_BC_uints = 0;
        int cnt_dates = 0;
        unsigned int x = 0;
        unsigned int BC = 0;
        unsigned int num_bits_translated = 0;
        unsigned int num_bits_invalid = 0;
        unsigned int num_bits_valid = UINT_SIZE;
        for (int cnt_dates_uints = 0; cnt_dates_uints < N; cnt_dates_uints++) {
            x = dates_uints[cnt_dates_uints];
            BC = 0;
            while (x) {
                BC += x & 1;
                x = x >> 1;
            }
            BC = BC % 2;
            x = dates_uints[cnt_dates_uints];
            if (cnt_dates_uints >= (cnt_BC_uints + 1) * UINT_SIZE)
                cnt_BC_uints++;
            if (BC == ((BC_uints[cnt_BC_uints] >> (cnt_dates_uints - cnt_BC_uints * UINT_SIZE)) & 1)) {
                if (num_bits_invalid)
                    x = x >> num_bits_invalid;
                if (num_bits_translated) {
                    if (num_bits_translated <= DAY_SIZE) {
                        dates[cnt_dates].day += (x << num_bits_translated) & DAY_MASK;
                        dates[cnt_dates].month = (x >> (DAY_SIZE - num_bits_translated)) & MONTH_MASK;
                        dates[cnt_dates].year = (x >> ((DAY_SIZE - num_bits_translated) + MONTH_SIZE)) & YEAR_MASK;
                    } else {
                        if (num_bits_translated <= DAY_SIZE + MONTH_SIZE) {
                            dates[cnt_dates].month += (x << (num_bits_translated - DAY_SIZE)) & MONTH_MASK;
                            dates[cnt_dates].year = (x >> (MONTH_SIZE - (num_bits_translated - DAY_SIZE))) & YEAR_MASK;
                        } else {
                            dates[cnt_dates].year += (x << (num_bits_translated - DAY_SIZE - MONTH_SIZE)) & YEAR_MASK;
                        }
                    }
                    if (dates[cnt_dates].day && dates[cnt_dates].month) {
                        dates[cnt_dates].year += UTC_START_YEAR;
                        cnt_dates++;
                    }
                    x = x >> (DATE_SIZE - num_bits_translated);
                    num_bits_valid -= DATE_SIZE;
                }
                num_bits_translated = num_bits_valid;
                while (num_bits_translated >= DATE_SIZE) {
                    dates[cnt_dates].day = x & DAY_MASK;
                    dates[cnt_dates].month = (x >> DAY_SIZE) & MONTH_MASK;
                    dates[cnt_dates].year = (x >> (DAY_SIZE + MONTH_SIZE)) & YEAR_MASK;
                    if (dates[cnt_dates].day && dates[cnt_dates].month) {
                        dates[cnt_dates].year += UTC_START_YEAR;
                        cnt_dates++;
                    }
                    x = x >> DATE_SIZE;
                    num_bits_translated -= DATE_SIZE;
                }
                if (num_bits_translated) {
                    dates[cnt_dates].day = x & DAY_MASK;
                    dates[cnt_dates].month = (x >> DAY_SIZE) & MONTH_MASK;
                    dates[cnt_dates].year = (x >> (DAY_SIZE + MONTH_SIZE)) & YEAR_MASK;
                }
                num_bits_invalid = 0;
                num_bits_valid = UINT_SIZE + num_bits_translated;
            } else {
                dates[cnt_dates].day = 0;
                dates[cnt_dates].month = 0;
                dates[cnt_dates].year = 0;
                num_bits_translated = 0;
                num_bits_invalid = DATE_SIZE - (num_bits_valid % DATE_SIZE ? (num_bits_valid % DATE_SIZE) : DATE_SIZE);
                num_bits_valid = UINT_SIZE - num_bits_invalid;
            }
        }
        qsort(dates, cnt_dates, sizeof(TDate), cmp_date);
        for (int i = 0; i < cnt_dates; i++)
            printf("%d %s %d\n", dates[i].day, MONTHS_NAMES[dates[i].month - 1], dates[i].year);
        free(dates);
    }
    return 0;
}
