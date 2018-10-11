#include <stdlib.h>
#include <float.h>

#include <stdarg.h>

#define inline __inline__

extern void recognize_init(void);
extern int vsprintf(char *s, const char *f, va_list arg);

#define G_MAXFLOAT FLT_MAX

#define g_malloc(size) malloc(size)
#define g_malloc0(size) calloc(size, 1)
#define g_realloc(ptr, size) realloc(ptr, size)
#define g_free(ptr) free(ptr)

#define g_warning(msg, ...) ((void) msg)
#define g_message(msg, ...) ((void) msg)
#define g_debug(msg, ...) ((void) msg)

#define g_ascii_isalpha(c) ((c) >= 'A' && (c) <= 'Z' || (c) >= 'a' && (c) <= 'z')
#define g_unichar_isgraph(c) ((c) > 0x20)

typedef int GTimer;

#define g_timer_new() ((GTimer *) 0)
#define g_timer_start(timer) ((void) 0)
#define g_timer_elapsed(timer, p) (*(p) = 0 )

typedef int gunichar;
typedef unsigned long gulong;

extern char *va(const char *format, ...);
