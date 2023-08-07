#!/bin/bash

set -xe

CFLAGS="-Wall -Wextra"
LIBS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"

clang $CFLAGS -o musicvw main.c $LIBS
