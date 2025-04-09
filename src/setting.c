// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "setting_manager.h"

void iam_setting_rewind(iam_id_t id) {
    iam__module_t *module = (iam__module_t *)id;
    module->current_setting = module->settings.head;    
}

iam_setting_t *iam_setting_read(iam_id_t id) {
    iam_setting_t *s;
    iam__module_t *module = (iam__module_t *)id;
    if (module->current_setting == NULL)
        return NULL;
    s = IAM_D(setting, module->current_setting);
    module->current_setting = module->current_setting->next;
    return s;
}

const char *iam_setting_to_str(iam_setting_t *s) {
    return s->info->type->to_str(s->setting);
}

const char *iam_setting_to_str_i(iam_setting_t *s, size_t i) {
    return s->info->type->to_str((char *)s->setting + s->info->size * i);
}

iam_setting_t *iam_setting_reg(iam_variable_t *v, void *value) {
    int res;
    iam__module_t *m;
    iam_setting_t *s = NULL;
    if (v != NULL) {
        s = IAM_NEW(setting);
        if (s == NULL)
            setting.status.init = IAM_OUT_OF_MEMORY;
        else {
            s->info = v;
            s->setting = value;
            m = (iam__module_t *)s->info->id;
            res = iam__list_append(&m->settings, s);
            if (res == 1) {
                setting.status.init = IAM_OUT_OF_MEMORY;
                return NULL;
            }
        } 
    }
    return s;
}

iam_variable_status iam_setting_set_max(iam_setting_t *s, size_t max) {
    return iam_variable_set_max(&setting, s->info, max, &s->setting);
}

void *iam_setting_get(iam_setting_t *s, size_t i, const iam_type_t *type) {
    iam_variable_status st;
    st = iam_variable_get_i_is_correct(&setting, s->info, i, &s->setting);
    if (st == IAM_VALUE_IS_CORRECT)
        return (char *)s->setting + s->info->size * i;
    return NULL;
}

void *iam_setting_set(iam_setting_t *s, size_t i, const iam_type_t *type,
        const void *value) {
    iam_variable_status st;
    st = iam_variable_set_i_is_correct(&setting, s->info, i, &s->setting);
    if (st == IAM_VALUE_IS_CORRECT)
        return (char *)s->setting + s->info->size * i;
    return NULL;
}