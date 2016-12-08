//
// Created by dev on 16-12-8.
//
#include <unistd.h>

#include <stdio.h>
#include <stdarg.h>

#ifndef LOG_TAG
#define LOG_TAG "RFTM"
#endif

#include <android/log.h>

#include "log.h"

int __log_level__ = LOG_INFO;

const char* log_level_names[] = {
        [LOG_VERBOSE] = "V",
        [LOG_DEBUG] = "D",
        [LOG_INFO] = "I",
        [LOG_WARN] = "W",
        [LOG_ERROR] = "E",
};

static const int map2droid[] = {
        [LOG_VERBOSE] = ANDROID_LOG_VERBOSE,
        [LOG_DEBUG] = ANDROID_LOG_DEBUG,
        [LOG_INFO] = ANDROID_LOG_INFO,
        [LOG_WARN] = ANDROID_LOG_WARN,
        [LOG_ERROR] = ANDROID_LOG_ERROR,
};

void log_set_level(int level)
{
    __log_level__ = level;
}

int log_format_print(int level, const char* fmt, ...)
{
    int ret = 0;
    va_list ap;
    va_start(ap, fmt);
    if (level >= __log_level__) {
        ret = vprintf(fmt, ap);
        __android_log_vprint(map2droid[level], LOG_TAG, fmt, ap);
    }
    va_end(ap);
    return ret;
}


