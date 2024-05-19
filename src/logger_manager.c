// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "logger_manager.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

iam__list_t iam__saved_logs;
void iam__saved_logs_free(void *data);
iam__list_t iam__log_stores;
iam__list_t iam__bufs;
char is_accumulation = 0;
iam_logger_level iam_logger_filter = IAM_LOG_LEVELS;

void iam__logger_manager_init(void) {
    is_accumulation = 1;
	iam__list_init(&iam__saved_logs);
	iam__list_init(&iam__log_stores);
}

void iam__logger_manager_exit(void) {
	iam__list_free_act(&iam__saved_logs, iam__saved_logs_free);
	iam__list_free(&iam__log_stores);
    iam__list_free(&iam__bufs);
}

static inline void iam__logger_put(iam_id_t id, iam_logger_level level,
    const char *msg);
static inline void iam__logger_print(const iam_log_t *log);

void iam_logger_puts(iam_id_t id, iam_logger_level level,
    const char *msg) {
    if (is_accumulation || iam__log_stores.count > 0 || IAM_CONSOLE) {
        iam__logger_put(id, level, msg);
    }
}

void iam_logger_putf(iam_id_t id, iam_logger_level level,
    const char *msg, ...) {
    va_list ap;
    int len, res;
    char buf[IAM_LOG_MAX_SIZE], *tmp;
    if (is_accumulation || iam__log_stores.count > 0 || IAM_CONSOLE) {
        va_start(ap, msg);
        vsprintf(buf, msg, ap);
        va_end(ap);
        len = strlen(buf) + 1;
        tmp = iam__malloc(len);
        if (tmp == NULL)
            return;
        memcpy(tmp, buf, len);
        res = iam__list_append(&iam__bufs, tmp);
        if (res == 1)
            return;
        iam__logger_put(id, level, tmp);
    }
}

int iam_logger_reg_save(iam_id_t id, iam_logger_level filter,
    iam_log_save_fn save) {
    int res;
    iam__log_store_t *store = IAM__NEW(log_store);
    if (store == NULL)
        return 1;
    store->id = (iam__module_t *)id;
    store->filter = filter;
    store->save = save;
    res = iam__list_append(&iam__log_stores, store);
    if (res == 1)
        return 2;
	iam_logger_puts(id, IAM_TRACE,
		"Registered a function for saving logs");
    return 0;
}

void iam__logger_put(iam_id_t id, iam_logger_level level,
    const char *msg) {
    iam__node_t *p;
    iam_log_t *log = IAM_NEW(log);
    if (log == NULL)
        return;
    log->id = id;
    log->time = time(NULL);
    log->level = level;
    log->msg = msg;
    if (IAM_CONSOLE && iam_logger_filter & level != 0)
        iam__logger_print(log);
    if (is_accumulation) {
        iam__list_append(&iam__saved_logs, log);
    } else {
        IAM__FOREACH(p, iam__log_stores)
            if (IAM__D(log_store, p)->filter & level != 0)
                IAM__D(log_store, p)->save(log);
    }
}

void iam__logger_print(const iam_log_t *log) {
    struct tm *now = localtime(&log->time);
    const char *type = "None";
    switch (log->level) {
        case IAM_ALL:   type="ALL";     break;
        case IAM_TRACE: type="TRACE";   break;
        case IAM_DEBUG: type="DEBUG";   break;
        case IAM_INFO:  type="INFO";    break;
        case IAM_WARN:  type="WARN";    break;
        case IAM_ERROR: type="ERROR";   break;
        case IAM_FATAL: type="FATAL";
    }
    printf("[%s] %02d.%02d.%04d %02d:%02d:%02d %s: %s\n", type,
        now->tm_mday, now->tm_mon+1, now->tm_year+1900,
        now->tm_hour, now->tm_min, now->tm_sec,
        log->id->info->name, log->msg);        
}

void iam__saved_logs_free(void *data) {
    iam__node_t *p;
    iam_log_t *log = (iam_log_t *)data;
    IAM__FOREACH(p, iam__log_stores)
        if (IAM__D(log_store, p)->filter & log->level != 0)
            IAM__D(log_store, p)->save(log);
}

void iam__logger_manager_flush(void) {
    iam__list_free_act(&iam__saved_logs, iam__saved_logs_free);
}