#include <stdio.h>
#include <stdarg.h>

static char lastError[512];

void setError(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(lastError, sizeof lastError, fmt, args);
    va_end(args);
}

const char* getLastError(void) {
    return lastError;
}