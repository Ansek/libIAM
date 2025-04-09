// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "setting_manager.h"
#include "plugin_manager.h"

iam__list_t iam__setting_stores;
IAM_VARIABLE_DEFINE_CLASS(setting);

void iam__setting_manager_init(void) {
    iam_variable_reset_status(&setting);
    iam__list_init(&iam__setting_stores);
}

void iam__setting_manager_exit(void) {
    iam__list_free(&iam__setting_stores);
}

void iam__setting_manager_load(void) {
    iam_id_t id, module;
    iam__node_t *p, *m;
    iam_callback_fn cb;
    iam_setting_load_fn load;
    iam_setting_dump_fn dump;
    IAM__FOREACH(p, iam__setting_stores) {
        id = IAM_D(setting_store, p)->id;
        if ((load = IAM_D(setting_store, p)->load) == NULL)
            continue;
        IAM__FOREACH(m, iam__plugins) {
            module = *IAM_D(id, &m);
            load(id, module);
            cb = ((iam__module_t *)module)->setting_cb;
            if (cb != NULL) {
                cb(module);
            }
        }
        if (dump = IAM_D(setting_store, p)->dump)
            dump(id);
    }
}

void iam_setting_reg_callback(iam_id_t id, iam_callback_fn fn) {
    iam__module_t *module = (iam__module_t *)id;
    module->setting_cb = fn;
}

iam_setting_store_t *iam_setting_reg_store(iam_id_t id) {
    int res;
    iam_setting_store_t *store = IAM_NEW(setting_store);
    if (store == NULL)
        return NULL;
    store->id = id;
    store->load = NULL;
    store->save = NULL;
    store->dump = NULL;
    res = iam__list_append(&iam__setting_stores, store);
    if (res == 1)
        return NULL;
	iam_logger_puts(id, IAM_TRACE,
		"Registered a setting store");
    return (iam_setting_store_t *)store;
}

void iam_setting_store_reg_save(iam_setting_store_t *store,
    iam_setting_save_fn fn) {
    store->save = fn;
	iam_logger_puts(store->id, IAM_TRACE,
		"Added save function (setting store)");
}

void iam_setting_store_reg_load(iam_setting_store_t *store,
    iam_setting_load_fn fn) {
    store->load = fn;
	iam_logger_puts(store->id, IAM_TRACE,
		"Added load function (setting store)");  
}

void iam_setting_store_reg_dump(iam_setting_store_t *store,
    iam_setting_dump_fn fn) {
    store->dump = fn;
	iam_logger_puts(store->id, IAM_TRACE,
		"Added dump function (setting store)");  
}