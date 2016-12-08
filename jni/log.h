//
// Created by dev on 16-12-8.
//

#ifndef RFTM_LOG_H
#define RFTM_LOG_H

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    LOG_VERBOSE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
};

extern const char* log_level_names[];

void log_set_level(int);
int log_format_print(int level, const char* fmt, ...);

#define log_print(level, fmt, ...) \
        log_format_print(level, "%s +%d @ %s: %d %d %s " fmt "\n", \
            __FILE__, __LINE__, __func__, getpid(), gettid(), log_level_names[level], ##__VA_ARGS__)

#define XLOGV(fmt, ...) log_print(VERBOSE, fmt, ##__VA_ARGS__)
#define XLOGD(fmt, ...) log_print(  DEBUG, fmt, ##__VA_ARGS__)
#define XLOGI(fmt, ...) log_print(   INFO, fmt, ##__VA_ARGS__)
#define XLOGW(fmt, ...) log_print(   WARN, fmt, ##__VA_ARGS__)
#define XLOGE(fmt, ...) log_print(  ERROR, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif //RFTM_LOG_H
