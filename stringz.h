#ifndef _STRINGX_H_
#define _STRINGX_H_

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "assertz.h"

// permit only single adjacent occurrences; squash out repeats via l-shift
int no_adj(char* buffer, char what)
{
    assert_ptr(buffer, __LINE__);
    assert_nz(*buffer, __LINE__);
    assert_nz(what, __LINE__);

    int len = strlen(buffer);
    char* pzero = buffer + len;
    int repeats = 0;

    for (int i = 0; i < len - repeats - 1; i++)
    {
        if ((buffer[i] == what) && (buffer[i+1] == what))
        {
            int delta = 1;
            char* p = buffer + i + delta;

            char* pmax = pzero - repeats;
            assert_z(*pmax, __LINE__);

            char* tmp = new char[len+1];

            while (p < pmax)
            {
                if (*p == 0)
                {
                    break;
                }

                if (*p == what)
                {
                    if (*(p+1) != what)
                    {
                        break;
                    }
                }

                delta++;
                p++;
            }

            strcpy(tmp, buffer + i + delta);
            *(buffer + i) = 0;
            strcat(buffer, tmp);
            delete[] tmp;

            repeats += delta;
        }
    }

    return repeats;
}

int replace_char(char* pstart, char* pstop, char what, char with)
{
    assert_ptr(pstart, __LINE__);
    assert_nz(*pstart, __LINE__);
    assert_nz(what, __LINE__);
    assert_nz(with, __LINE__);

    int reps = 0;

    char* p1 = pstart;
    char* p2 = (pstop) ? pstop : pstart + strlen(pstart);

    while (p1 <= p2)
    {
        if (*p1 == what)
        {
            *p1 = with;
            reps++;
        }

        p1++;
    }

    return reps;
}

int remove_trailing_whitespace(char* p)
{
    int ws = 0;
    int lenp = strlen(p);

    if (lenp)
    {
        char* plast = p + lenp - 1;

        while (plast > p)
        {
            if ((*plast == ' ') || (*plast == '\t'))
            {
                *plast = 0;
                plast--;
                ws++;
            }
            else
            {
                break;
            }
        }
    }

    return ws;
}

#endif
