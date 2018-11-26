#ifndef _PRINT_H_
#define _PRINT_H_

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "assertz.h"

#define print_stderr pre
#define print_stdout pro

// Print to stderr, with newline; invoke as pre / print_stderr
void pre(const char* __restrict format, ...)
{
    va_list va;

    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);
    fprintf(stderr, "\n");
}

// Print to stdout, with newline; invoke as pro / print_stdout
void pro(const char* __restrict format, ...)
{
    va_list va;

    va_start(va, format);
    vfprintf(stdout, format, va);
    va_end(va);
    fprintf(stdout, "\n");
}

// Print to fd
int print(int fd, const char* p, bool newline)
{
    assert_ptr(p, __LINE__);
    assert_gt(fd, 0, __LINE__);

    int len = strlen(p);
    int i = 0;
    int result = 0;

    while (i < len)
    {
        result = write(fd, p+i, 1);
        assert_gt(result, 0, __LINE__);
        result = 0;
        i++;
    }

    if (newline)
    {
        char eol[] = { 10, 0 };
        char* peol = &(eol[0]);

        while (*peol)
        {
            result = write(fd, peol, 1);
            assert_gt(result, 0, __LINE__);
            result = 0;
            i++;
            peol++;
        }
    }

    return i;
}

#endif
