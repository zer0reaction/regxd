#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "table.h"
#include "compiled.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define DEBUG_BYTE 0xCE

typedef enum {
    TOKEN_ERROR = 0,
    TOKEN_CH,                   // [x] character (includes escaped ones)
    TOKEN_SBR_OPEN,             // [ ] [
    TOKEN_SBR_CLOSE,            // [ ] ]
    TOKEN_SBR_CARET_OPEN,       // [ ] [^
    TOKEN_MINUS,                // [x] -
    TOKEN_DOT,                  // [x] .
} Token_Type;

typedef struct {
    Token_Type type;

    struct {
        char value;
    } ch;
} Token;

typedef enum {
    NODE_ERROR = 0,
    NODE_CH,
    NODE_INCLUDE_CLASS,
    NODE_EXCLUDE_CLASS,
    NODE_RANGE,
    NODE_ANYCH,
} Node_Type;

typedef struct Node Node;
struct Node {
    Node_Type type;
    Node *next;

    struct {
        char value;
    } ch;

    struct {
        Node *head;
    } include_class;

    struct {
        Node *head;
    } exclude_class;

    struct {
        char start;
        char end;
    } range;
};

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

size_t chop_token(Token *t, const char *str)
{
    size_t matched = -1;

    if ((matched = match(&table_ch, str))) {
        t->type = TOKEN_CH;
        (matched == 1) ? (t->ch.value = str[0]) : (t->ch.value = str[1]);
        return matched;
    }

    if ((matched = match(&table_sbr_caret_open, str))) {
        t->type = TOKEN_SBR_CARET_OPEN;
        return matched;
    }

    if ((matched = match(&table_sbr_open, str))) {
        t->type = TOKEN_SBR_OPEN;
        return matched;
    }

    if ((matched = match(&table_sbr_close, str))) {
        t->type = TOKEN_SBR_CLOSE;
        return matched;
    }

    if ((matched = match(&table_minus, str))) {
        t->type = TOKEN_MINUS;
        return matched;
    }

    if ((matched = match(&table_dot, str))) {
        t->type = TOKEN_DOT;
        return matched;
    }

    return 0;
}

size_t chop_node(Node *n, Token *ts, size_t eaten)
{
    if (eaten + 2 < arrlenu(ts) && ts[eaten + 1].type == TOKEN_MINUS) {
        if (ts[eaten].type != TOKEN_CH || ts[eaten + 2].type != TOKEN_CH) {
            return 0;
        }
        n->type = NODE_RANGE;
        n->range.start = ts[eaten].ch.value;
        n->range.end = ts[eaten + 2].ch.value;
        return 3;
    }

    if (eaten < arrlenu(ts) && ts[eaten].type == TOKEN_CH) {
        n->type = NODE_CH;
        n->ch.value = ts[eaten].ch.value;
        return 1;
    }

    if (eaten < arrlenu(ts) && ts[eaten].type == TOKEN_DOT) {
        n->type = NODE_ANYCH;
        return 1;
    }

    assert(0 && "not implemented yet");

    return 0;
}

size_t compile(Table *t, const char *str, size_t len, Arena *a)
{
    Token *ts = NULL;
    Node *head = NULL;
    Node *tail = NULL;

    {
        size_t proc = 0;

        while (proc < len) {
            size_t n = -1;
            Token tok = {0};

            n = chop_token(&tok, &str[proc]);
            if (!n) {
                printf("Error compiling regex %s: syntax error on char %lu ('%c').\n", str, proc + 1, str[proc]);
                return 0;
            }
            assert(proc + n <= len);
            arrput(ts, tok);
            proc += n;
        }
    }

    {
        size_t eaten = 0;

        while (eaten < arrlenu(ts)) {
            size_t n = -1;
            Node *node = arena_alloc(a, sizeof *node);
            memset(node, DEBUG_BYTE, sizeof *node);

            n = chop_node(node, ts, eaten);
            assert(n);
            assert(eaten + n <= arrlenu(ts));

            if (!tail) {
                assert(head == NULL);
                head = tail = node;
            } else {
                tail->next = node;
                tail = node;
            }

            eaten += n;
        }
    }

    arrfree(ts);
    return len;
}

int main(void)
{
    Table t = {0};
    Arena a = {0};
    const char *str = "a-z";

    compile(&t, str, strlen(str), &a);

    arena_free(&a);
    return 0;
}
