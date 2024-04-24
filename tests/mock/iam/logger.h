// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_LOGGER_H__
#define __IAM_LOGGER_H__

#include <iam/iam.h>
#include <time.h>
#include <fff.h>

typedef enum {
    IAM_ALL, 
    IAM_TRACE, 
    IAM_DEBUG, 
    IAM_INFO, 
    IAM_WARN, 
    IAM_ERROR, 
    IAM_FATAL
} iam_logger_level_t;

typedef struct {
    time_t time;
    iam_id_t *handle;
    iam_logger_level_t level;
    const char *msg;
} iam_log_t;

typedef void (*iam_log_save_fn)(iam_log_t *log);

#define IAM_LOG_MAX_SIZE 512
#define IAM_LOG_LEVELS IAM_ALL

DECLARE_FAKE_VOID_FUNC2(iam_logger_put, iam_id_t *, iam_logger_level_t);

void iam_logger_puts(iam_id_t *handle, iam_logger_level_t level,
    const char* msg);
void iam_logger_putf(iam_id_t *handle, iam_logger_level_t level,
    const char* msg, ...);

int iam_logger_reg_save(iam_id_t *handle, iam_logger_level_t filter,
    iam_log_save_fn save);
#endif