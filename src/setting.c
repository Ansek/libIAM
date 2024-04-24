// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "setting_manager.h"

typedef struct {
    iam_setting_t info;
    void *value;
} iam__setting_t;

void iam_setting_rewind(iam_id_t *handle) {
    iam__module_t *module = (iam__module_t *)handle;
    module->current_setting = module->settings.head;    
}

iam_setting_t *iam_setting_read(iam_id_t *handle) {
    iam__module_t *module = (iam__module_t *)handle;
    if (module->current_setting == NULL)
        return NULL;
    iam__setting_t *stg = IAM__D(setting, module->current_setting);
    module->current_setting = module->current_setting->next;
    return (iam_setting_t *)stg;
}

#define IAM_SETTING_REG(E, S)                           \
    iam__setting_t *s = IAM__NEW(setting);              \
    if (s == NULL)                                      \
        return;                                         \
    s->info.name = name;                                \
    s->info.type = E;                                   \
    s->info.max.size = S;                               \
    s->value = ref;                                     \
    iam__module_t *module = (iam__module_t *)handle;    \
    iam__list_append(&module->settings, s)                              

#define IAM_SETTING_FUNC(T, E)                          \
    void iam_setting_reg_##T(iam_id_t *handle,          \
            const char *name, T##_t *ref) {             \
        IAM_SETTING_REG(E, 1);                          \
    }                                                   \
    void iam_setting_reg_##T##_arr(iam_id_t *handle,    \
            const char *name, T##_t *ref, size_t max) { \
        IAM_SETTING_REG(E, max);                        \
    }

#define float_t float
#define ufloat_t float
#define double_t double
#define udouble_t double

IAM_SETTING_FUNC(int8, IAM_INT8);
IAM_SETTING_FUNC(int16, IAM_INT16);
IAM_SETTING_FUNC(int32, IAM_INT32);
IAM_SETTING_FUNC(int64, IAM_INT64);
IAM_SETTING_FUNC(uint8, IAM_UINT8);
IAM_SETTING_FUNC(uint16, IAM_UINT16);
IAM_SETTING_FUNC(uint32, IAM_UINT32);
IAM_SETTING_FUNC(uint64, IAM_UINT64);
IAM_SETTING_FUNC(float, IAM_FLOAT);
IAM_SETTING_FUNC(double, IAM_DOUBLE);
IAM_SETTING_FUNC(ufloat, IAM_UFLOAT);
IAM_SETTING_FUNC(udouble, IAM_UDOUBLE);
