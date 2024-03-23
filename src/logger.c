// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "iam/logger.h"

/*! Фильтр уровней сообщений
*/
char iam_logger_filter = IAM_LOG_LEVELS;

static inline void iam__logger_print(iam_id_t *handle, iam_logger_level_t level,
    const char *msg);

void iam_logger_puts(iam_id_t *handle, iam_logger_level_t level,
    const char *msg) {
    if (iam_logger_filter & level == 0)
        return;
    iam__logger_print(handle, level, msg);
}

void iam_logger_putf(iam_id_t *handle, iam_logger_level_t level,
    const char *msg, ...) {
    if (iam_logger_filter & level == 0)
        return;
    char buf[IAM_LOG_MAX_SIZE];
    va_list ap;
    va_start(ap, msg);
    vsprintf(buf, msg, ap);
    va_end(ap);
    iam__logger_print(handle, level, buf);
}

void iam__logger_print(iam_id_t *handle, iam_logger_level_t level,
    const char *msg) {
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    const char* type = "None";
    switch (level) {
        case IAM_ALL:   type="ALL";     break;
        case IAM_TRACE: type="TRACE";   break;
        case IAM_DEBUG: type="DEBUG";   break;
        case IAM_INFO:  type="INFO";    break;
        case IAM_WARN:  type="WARN";    break;
        case IAM_FATAL: type="FATAL";
    }
    printf("[%s] %02d.%02d.%04d %02d:%02d:%02d %s: %s\n", type,
        now->tm_mday, now->tm_mon+1, now->tm_year+1900,
        now->tm_hour, now->tm_min, now->tm_sec,
        handle->name, msg);
}