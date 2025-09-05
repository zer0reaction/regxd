#!/bin/bash

CC='gcc'
WARNFLAGS='-Wall -Wextra -Wconversion'
CFLAGS='-O0 -std=c99 -ggdb'

set -xe

${CC} ${WARNFLAGS} ${CFLAGS} -o main main.c
