#ifndef TABLE_H_
#define TABLE_H_

#define ROWS_COUNT 32
#define EVENTS_COUNT 128

#define STATE_FAIL 0
#define STATE_INIT 1

typedef struct {
    uint32_t rows;
    uint32_t data[ROWS_COUNT][EVENTS_COUNT];
} Table;

#endif // TABLE_H_
