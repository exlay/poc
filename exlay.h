#define DAEMON_PORT 11017

#ifdef DEBUG
#define debug_printf(fmt, ...) fprintf(stderr, "%s(%d)debug: " fmt, __func__, __LINE__, ## __VA_ARGS__)
#else
#define debug_printf(fmt, ...)
#endif
