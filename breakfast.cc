#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <string>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "assertz.h"
#include "printz.h"
#include "stringz.h"

#define BLOCKSIZE 512

void usage()
{
    pre("Usage: breakfast [--linelength=N] <file>");
    pre("32 <= N <= 128. If linelength is not specified, the default is 72");
    pre("If <file> is not given, standard input is read");

    exit(1);
}

struct textblock
{
    char text[BLOCKSIZE];   // null-terminated
    int bs;                 // how many bytes can be read before the null char
    textblock* next;
};

textblock* newnode(textblock** pp)
{
    assert_ptr(pp, __LINE__);
    assert_nullptr(*pp, __LINE__);

    *pp = new(std::nothrow) textblock;
    assert_ptr(*pp, __LINE__);

    (*pp)->next = 0;
    memset((*pp)->text, 0, sizeof((*pp)->text));
    (*pp)->bs = 0;

    return *pp;
}

int main(int argc, const char* argv[])
{
    struct stat filestat;
    char* filepath = 0;
    int filepathlen = 1024;

    const char* hszlen = "--linelength=";
    int LINELENGTH = 72;

    argc--;

    while (argc > 0)
    {
        if (strncmp(argv[argc], hszlen, strlen(hszlen)) == 0)
        {
            char length[8];

            const char* p_equals = strchr(argv[argc], '=');
            assert_ptr(p_equals, __LINE__, (long) &usage);
            assert_nz(*(p_equals+1), __LINE__, (long) &usage);

            strcpy(length, p_equals + 1);
            length[sizeof(length)-1] = 0;
            assert_num(length, __LINE__, (long) &usage);

            LINELENGTH = atoi(length);
            assert_ge(LINELENGTH, 32, __LINE__, (long) &usage);
            assert_le(LINELENGTH, 128, __LINE__, (long) &usage);
        }
        else if (stat(argv[argc], &filestat) == 0)
        {
            assert_nz(S_ISREG(filestat.st_mode), __LINE__, (long) &usage);

			//Before allocating, delete any memory taken by argv[argc+N]
			delete[] filepath;
			filepath = 0;

            filepath = new(std::nothrow) char[filepathlen];
            assert_ptr(filepath, __LINE__);

            strcpy(filepath, argv[argc]);
            filepath[filepathlen-1] = 0;
        }
        else
        {
            usage();
        }

        argc--;
    }

    int fi = 0;

    if (filepath)
    {
        fi = open(filepath, O_RDONLY);
    }
    else
    {
        fi = dup(0);
    }

    assert_gt(fi, 2, __LINE__);

    int result = 0;
    char* data = 0;

    textblock* pHead = 0;       // our linked list's head
    textblock* pNode = 0;       // our linked list's iterator

    int n = 0;                  // bytes written to the current struct
    int N = 0;                  // bytes written to all structs, starting pHead

    bool keep_reading = true;

    while (keep_reading)
    {
        if (! data)
        {
            data = new(std::nothrow) char[BLOCKSIZE];
            assert_ptr(data, __LINE__);
            memset(data, 0, BLOCKSIZE);
            n = 0;
        }

        result = read(fi, data + n, 1);

        if (result < 1)
        {
            keep_reading = false;
        }
        else
        {
            n++;
            N++;
        }

        if ((n == BLOCKSIZE - 1) || (keep_reading == false))
        {
            if (! pHead)
            {
                pHead = newnode(&pNode);
            }
            else
            {
                assert_nullptr(pNode->next, __LINE__);
                pNode = newnode(&(pNode->next));
            }

            strncpy(pNode->text, data, n);
            pNode->bs = n;

            delete[] data;
            data = 0;
        }
    }

    close(fi);

    char* consolidated_text = new(std::nothrow) char[N+1];
    assert_ptr(consolidated_text, __LINE__);
    memset(consolidated_text, 0, N+1);

    textblock* ptb = pHead;

    while (ptb)
    {
        strcat(consolidated_text, ptb->text);
        ptb = ptb->next;
    }

    ptb = pHead;

    while (ptb)
    {
        textblock* pnext = ptb->next;
        delete ptb;
        ptb = pnext;
    }

    pHead = 0;
    pNode = 0;

    int i = 0;
    int paras = 0;

    char* p = consolidated_text;
    char* p2 = 0;

    while ((p2 = strstr(p, "\n\n")))
    {
        replace_char(p+1, p2-1, '\n', ' ');
        p = p2 + 2;
        paras++;
    }

    if (*p)
    {
        replace_char(p+1, 0, '\n', ' ');
        remove_trailing_whitespace(p);
        paras++;
    }

    int fo = dup(1);
    assert_gt(fo, 2, __LINE__);

    no_adj(consolidated_text, ' ');
    no_adj(consolidated_text, '\t');

    p = consolidated_text;
    p2 = 0;

    char* buffer = new(std::nothrow) char[N+1];
    assert_ptr(buffer, __LINE__);
    memset(buffer, 0, N+1);

    int j = 0;
    int len = 0;

    while (j < paras)
    {
        p2 = strstr(p, "\n\n");

        if (p2)
        {
            *p2 = 0;
            strcpy(buffer, p);
            buffer[N] = 0;
            p = p2 + 2;
        }
        else
        {
            strcpy(buffer, p);
            buffer[N] = 0;
        }

        char* p0 = buffer;
        len = strlen(p0);

        char* buff = new(std::nothrow) char[LINELENGTH+1];
        assert_ptr(buff, __LINE__);

        while (len > LINELENGTH)
        {
            memset(buff, 0, LINELENGTH+1);

            bool b_ins = false;
            char* p3 = p0 + LINELENGTH - 1;

            while (p3 > p0)
            {
                if (isspace(*p3))
                {
                    strncpy(buff, p0, p3-p0);
                    b_ins = true;
                    p0 = p3+1;
                    break;
                }

                p3--;
            }

            if (! b_ins)
            {
                strncpy(buff, p0, LINELENGTH);
                p0 += LINELENGTH;
            }

            print(fo, buff, ((*p0 == '\n') ? false : true));
            len = strlen(p0);
        }

        delete[] buff;

        print(fo, p0, false);
        print(fo, (j < paras - 1) ? "\n\n" : "\n", false);
        j++;
    }

    close(fo);

    delete[] consolidated_text;
    delete[] buffer;
    delete[] filepath;

    return 0;
}
