#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define ROWS_COUNT 32
#define EVENTS_COUNT 128

#define STATE_FAIL 0
#define STATE_INIT 1

typedef struct {
    uint32_t rows;
    uint32_t data[ROWS_COUNT][EVENTS_COUNT];
} Table;

void compile(Table *t, const char *str)
{
    uint32_t state = STATE_INIT;
    bool in_cc = false;

    while (str[0]) {
        assert(state < ROWS_COUNT && "too many states");

        if (str[0] == '[') {
            in_cc = true;
            str++;
            continue;
        }

        if (str[0] == ']') {
            assert(in_cc && "stray ']' found");
            in_cc = false;
            str++;
            state++;
            continue;
        }

        if (in_cc) {
            t->data[state][(uint8_t)str[0]] = state + 1;
            str++;
            continue;
        }

        if (str[0] == '$') {
            t->data[state][0] = state + 1;
        } else {
            t->data[state][(uint8_t)str[0]] = state + 1;
        }

        str++;
        state++;
    }

    assert(!in_cc && "character class not closed");

    t->rows = state - STATE_INIT + 1;
}

void dump(Table *t)
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

void dump_array(Table *t)
{
    size_t i, j;

    printf("static uint32_t <compiled> = {\n");
    for (i = 0; i < t->rows; i++) {
        printf("    { ");
        for (j = 0; j < EVENTS_COUNT; j++) {
            printf("%u, ", t->data[i][j]);
        }
        printf("},\n");
    }
    printf("\n};\n");
}

bool match(Table *t, const char *str)
{
    uint32_t state = STATE_INIT;

    while (1) {
        if (state == STATE_FAIL || state >= t->rows) {
            break;
        }
        state = t->data[state][(uint8_t)str[0]];
        str++;
    }

    return state != STATE_FAIL;
}

int main(void)
{
    Table t = {0};

    compile(&t, "if[ \n\t(]");
    dump_array(&t);

    return 0;
}
