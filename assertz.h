#ifndef _ASSERTX_H_
#define _ASSERTX_H_

#include <stdio.h>
#include <stdlib.h>

void callback(long cb)
{
    typedef void (*pfx)(void);

    if (cb)
    {
        pfx ptr = (pfx) cb;
        (*ptr)();
    }
}

void assert_bool(bool arg, int line, long cb = 0)
{
    if (! arg)
    {
        fprintf(stderr, "Aborting ! Runtime check failed at line %d\n", line);

        if (cb)
        {
            callback(cb);
        }

        exit(1);
    }
}

void assert_z(long arg, int line, long cb = 0)
{
    assert_bool((arg == 0), line, cb);
}

void assert_nz(long arg, int line, long cb = 0)
{
    assert_bool((arg != 0), line, cb);
}

void assert_ptr(const void* arg, int line, long cb = 0)
{
    assert_bool(((long) arg >= sizeof(void*)), line, cb);
}

void assert_nullptr(const void* arg, int line, long cb = 0)
{
    assert_bool(((long) arg == 0), line, cb);
}

void assert_gt(long arg, long floor, int line, long cb = 0)
{
    assert_bool((arg > floor), line, cb);
}

void assert_ge(long arg, long floor, int line, long cb = 0)
{
    assert_bool((arg >= floor), line, cb);
}

void assert_lt(long arg, long ceiling, int line, long cb = 0)
{
    assert_bool((arg < ceiling), line, cb);
}

void assert_le(long arg, long ceiling, int line, long cb = 0)
{
    assert_bool((arg <= ceiling), line, cb);
}

void assert_eq(long arg, long other, int line, long cb = 0)
{
    assert_bool((arg == other), line, cb);
}

void assert_ne(long arg, long other, int line, long cb = 0)
{
    assert_bool((arg != other), line, cb);
}

void assert_num(const char* p, int line, long cb = 0)
{
    int max = strlen(p);

    for (int i = 0; i < max; i++)
    {
        assert_bool(isdigit(p[i]), line, cb);
    }
}

#endif
