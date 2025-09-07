#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "table.h"
#include "compiled.h"

void dump(const Table *t)
{
    size_t i, j;

    for (i = 0; i < EVENTS_COUNT; i++) {
        printf("%3lu :: ", i);
        for (j = 0; j < t->rows; j++) {
            printf("%3d", t->data[j][i]);
        }
        printf("\n");
    }
}

void dump_compiled(const Table *t)
{
    size_t i, j;

    printf("static const Table <compiled> = {\n");
    printf("    .rows = %u,\n", t->rows);
    printf("    .data = {\n");

    for (i = 0; i < t->rows; i++) {
        printf("        {\n");
        printf("            ");

        for (j = 0; j < EVENTS_COUNT; j++) {
            if ((j + 1) % 16 == 0) {
                printf("%d,", t->data[i][j]);
            } else {
                printf("%d, ", t->data[i][j]);
            }

            if ((j + 1) % 16 == 0) {
                printf("\n");
                if (j != EVENTS_COUNT - 1) printf("            ");
                else                       printf("        ");
            }
        }
        printf("},\n");
    }
    printf("    }\n");
    printf("};\n");
}

size_t match(const Table *t, const char *str)
{
    const char *str_start = str;
    State state = STATE_INIT;

    while (1) {
        bool b = false;

        b = (b || state == STATE_FAIL);
        b = (b || state >= (State)t->rows);
        b = (b || state == STATE_NOCONSUME);
        if (b) break;

        state = t->data[state][(uint8_t)str[0]];
        if (state != STATE_NOCONSUME) str++;
    }

    if (state == STATE_FAIL) {
        return 0;
    } else {
        return (size_t)(str - str_start);
    }
}

int main(void)
{
    return 0;
}
