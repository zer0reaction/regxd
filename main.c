#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "table.h"
#include "compiled.h"

typedef enum {
    TOKEN_ERROR = 0,
    TOKEN_CH,                   // character (includes escaped ones)
    TOKEN_SBR_OPEN,             // [
    TOKEN_SBR_CLOSE,            // ]
    TOKEN_SBR_CARET_OPEN,       // [^
    TOKEN_MINUS,                // -
    TOKEN_DOT,                  // .
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

void dump_array(const Table *t)
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
    const Table *t = &table_ch;

    {
        size_t i = (size_t)-1;
        const char *str = "int\\[f\\]oo\\]b\\.a\\-r\\\\oooo";
        size_t len = (size_t)-1;

        len = strlen(str);

        i = 0;
        while (i < len) {
            size_t n = match(t, &str[i]);
            printf("%s :: %lu\n", &str[i], n);
            i += n;
        }
    }

    return 0;
}
