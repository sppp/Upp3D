#include "ShaderCore.h"


NAMESPACE_SHADER_BEGIN

// An implementation of vasprintf
int allocvfmt(char **str, const char *fmt, va_list vp) {
    int size = 0;
    va_list va;
    va_copy(va, vp);
    size = vsnprintf(0, size, fmt, va);
    va_end(va);

    if (size < 0)
        return -1;
    *str = (char *)malloc(size + 1);
    if (!*str)
        return -1;
    return vsprintf(*str, fmt, vp);
}

// An implementation of vsprintf
int allocfmt(char **str, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int size = allocvfmt(str, fmt, va);
    va_end(va);
    return size;
}

NAMESPACE_SHADER_END
