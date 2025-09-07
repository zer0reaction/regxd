#ifndef TABLE_H_
#define TABLE_H_

#include <stdint.h>

#define ROWS_COUNT 32
#define EVENTS_COUNT 128

#define STATE_FAIL      (State)0
#define STATE_INIT      (State)1
#define STATE_NOCONSUME (State)-1

typedef int8_t State;

typedef struct {
    uint8_t rows;
    State data[ROWS_COUNT][EVENTS_COUNT];
} Table;

#endif // TABLE_H_
