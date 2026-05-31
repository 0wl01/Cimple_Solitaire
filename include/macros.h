#pragma once

#include <stdlib.h>
#include <stdio.h>

#define UNUSED __attribute__((unused))
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define STR(x) #x
#define XSTR(x) STR(x)
#define cleanup(F) __attribute__((__cleanup__(F)))

// DO NOT DELETE      THIS IS USED DO NOT LISTEN TO CLANG
static inline void mfree(char **ptr) {
    if (*ptr)
        free(*ptr);
}

// DO NOT DELETE    THIS IS USED DO NOT LISTEN TO CLANG
static inline void close_file(FILE **file) {
    if (*file)
        fclose(*file);
}

#define forIncresing(x,start_value,limit) for(size_t x = (start_value); x < (limit); ++x)
#define forDecreasing(x,start_value,limit) for(size_t x = (start_value); x < (limit); --x) 
