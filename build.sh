#!/bin/bash

gcc -std=c17 -Wall -Werror -Wpedantic -Wextra -Wfloat-equal -Wfloat-conversion -Wvla -o voronoi main.c
