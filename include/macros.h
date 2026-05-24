#pragma once

#define UNUSED __attribute__((unused))
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define STR(x) #x
#define XSTR(x) STR(x)
#define cleanup(F) __attribute__((__cleanup__(F)))
