// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "setting_manager.h"

iam__list_t iam_modules;

iam__list_t iam__setting_stores;

void iam__setting_manager_init(void) {
    iam__list_init(&iam_modules);
    iam__list_init(&iam_modules);
}

void iam__setting_manager_exit(void) {
    iam__list_clear(&iam_modules);
    iam__list_free(&iam__setting_stores);
}

void iam__setting_manager_load(void) {
    
}

iam_setting_store_t *iam_setting_reg_store(iam_id_t *handle) {
    int res;
    iam__setting_store_t* stg = IAM__NEW(setting_store);
    if (stg == NULL)
        return NULL;
    stg->handle = handle;
    stg->store.load = NULL;
    stg->store.load = NULL;
    res = iam__list_append(&iam__setting_stores, stg);
    if (res == 1)
        return NULL;
	iam_logger_puts(handle, IAM_TRACE,
		"Registered a setting store");
    return (iam_setting_store_t *)stg;
}

void iam_setting_store_reg_save(iam_setting_store_t *handle,
    iam_setting_save_fn fn) {
    handle->save = fn;
	iam_logger_puts(IAM__HANDLE(setting_store), IAM_TRACE,
		"Added save function (setting store)");
}

void iam_setting_store_reg_load(iam_setting_store_t *handle,
    iam_setting_load_fn fn) {
    handle->load = fn;
	iam_logger_puts(IAM__HANDLE(setting_store), IAM_TRACE,
		"Added load function (setting store)");  
}