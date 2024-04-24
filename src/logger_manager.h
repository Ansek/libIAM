// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_LOGGER_MANAGER_H__
#define __IAM_LOGGER_MANAGER_H__

#include <iam/logger.h>
#include <common.h>

#ifdef IAM_CONSOLE_LOG
    #define IAM_CONSOLE 1
#else
    #define IAM_CONSOLE 0
#endif

typedef struct {
    iam_id_t *handle;
    iam_logger_level_t filter;
    iam_log_save_fn save;
} iam__log_store_t;

void iam__logger_manager_init(void);
void iam__logger_manager_exit(void);
void iam__logger_manager_flush(void);

#endif