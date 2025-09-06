#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "table.h"
#include "compiled.h"

typedef enum {
    TOKEN_ERROR = 0,
    TOKEN_CH,                   // [x] character (includes escaped ones)
    TOKEN_SBR_OPEN,             // [x] [
    TOKEN_SBR_CLOSE,            // [x] ]
    TOKEN_SBR_CARET_OPEN,       // [x] [^
    TOKEN_MINUS,                // [x] -
    TOKEN_DOT,                  // [x] .
} Token_Type;

typedef struct {
    Token_Type type;

    struct {
        char value;
    } ch;
} Token;

void compile(Table *t, const char *str)
{
    assert(0 && "todo");
    (void)t;
    (void)str;

    #if 0
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
    #endif
}

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
        printf("        { ");
        for (j = 0; j < EVENTS_COUNT; j++) {
            printf("%u, ", t->data[i][j]);
        }
        printf("},\n");
    }
    printf("    }\n");
    printf("};\n");
}

size_t match(const Table *t, const char *str)
{
    const char *str_start = str;
    uint32_t state = STATE_INIT;

    while (1) {
        if (state == STATE_FAIL || state >= t->rows) {
            break;
        }
        state = t->data[state][(uint8_t)str[0]];
        str++;
    }

    if (state == STATE_FAIL) {
        return 0;
    } else {
        return (size_t)(str - str_start);
    }
}

int main(void)
{
    #if 0
    Table t = {0};

    t.rows = 2;
    t.data[1]['.'] = 2;

    dump_compiled(&t);
    #endif

    const char *str = ".";
    printf("%lu\n", match(&table_dot, str));

    return 0;
}
