#include "config.h"
#include <string.h>

extern char *profile_buffer;
extern int profile_size;

int profile_line;

static char tempstring[4096];

char *profile_read() {
    tempstring[0] = '\0';
    return &tempstring[0];
}

void profile_write(char *text)
{
    /* TODO */
}

void profile_sync_int(int *intptr)
{
    /* TODO */
}

char *va(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    vsprintf(tempstring, msg, args);
    va_end(args);
    return &tempstring[0];
}

