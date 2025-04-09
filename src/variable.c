// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <iam/variable.h>
#include <common.h>
#include <inttypes.h>
#include <string.h>

#define IAM__BS 128
#define MAX_SYM_UINT64 20

char iam__var_buf[IAM__BS];
size_t iam__var_i = 0;

#define IAM__TO_STR(N, T)                           \
    char *str = iam__var_buf + iam__var_i;          \
    iam__var_i += sprintf(str, N->spec, *(T *)val); \
    if (iam__var_i + MAX_SYM_UINT64 >= IAM__BS)     \
        iam__var_i = 0;                             \
    return str

#define IAM__VAR_BOOLEAN(...) return *(bool *)val ? "true" : "false"
#define IAM__VAR_INTEGER(N, T) IAM__TO_STR(N, T)
#define IAM__VAR_REAL(N, T) IAM__TO_STR(N, T)
#define IAM__VAR_STRING(...) return (char *)val

#define IAM__VAR_INIT_TYPE(T, N, S, U, C)           \
    const char *IAM__##N##_TO_STR(const void *val) {\
        IAM__VAR_##C(IAM_##N, T);                   \
    }                                               \
    iam_type_t iam__##N = {                         \
        .name = #T,                                 \
        .spec = "%"S,                               \
        .is_unsigned = U,                           \
        .category = IAM_##C,                        \
        .to_str = IAM__##N##_TO_STR,                \
        .size = (uint8_t)sizeof(T)                  \
    };                                              \
    const iam_type_t *const IAM_##N = &iam__##N

#define IAM__VAR_INIT_INT(n)                                    \
    IAM__VAR_INIT_TYPE(int##n##_t, INT##n, PRId##n, 0, INTEGER);\
    IAM__VAR_INIT_TYPE(uint##n##_t, UINT##n, PRIu##n, 1, INTEGER)

#define IAM__VAR_INIT_REAL(T, N, S)         \
    IAM__VAR_INIT_TYPE(T, N, S, 0, REAL);   \
    IAM__VAR_INIT_TYPE(T, U##N, S, 1, REAL)

IAM__VAR_INIT_TYPE(bool, BOOL, PRIu8, 0, BOOLEAN);
IAM__VAR_INIT_INT(8);
IAM__VAR_INIT_INT(16);
IAM__VAR_INIT_INT(32);
IAM__VAR_INIT_INT(64);
IAM__VAR_INIT_REAL(float, FLOAT, IAM_FLOAT_SPEC);
IAM__VAR_INIT_REAL(double, DOUBLE, IAM_DOUBLE_SPEC);
IAM__VAR_INIT_TYPE(const char *, STR, "s", 0, STRING);

void iam_variable_reset_last_status(iam_class_t *c) {
    IAM_VFF_RESET(c, last, reg);
    IAM_VFF_RESET(c, last, get);
    IAM_VFF_RESET(c, last, set);
}

void iam_variable_reset_high_status(iam_class_t *c) {
    IAM_VFF_RESET(c, high, reg);
    IAM_VFF_RESET(c, high, get);
    IAM_VFF_RESET(c, high, set);
}

void iam_variable_reset_status(iam_class_t *c) {
    iam_variable_reset_high_status(c);
    iam_variable_reset_high_status(c);
    c->status.init = IAM_SUCCESS_INIT;
}

#define IAM__SET_STATUS(m, s) do {              \
    c->status.last.m = s;                       \
    if (c->status.high.m < s)                   \
        c->status.high.m = s;                   \
    c->status.init = IAM_SETTING_HAS_IGNORED;   \
} while (0)

iam_variable_t *iam_variable_reg(iam_class_t *c, iam_id_t id,
    const iam_type_t *type, const char *name, const char *desc,
    void *ref, size_t size, size_t max) {
    iam_variable_t *v;
    if (ref == NULL && max == 1) {
        IAM__SET_STATUS(reg, IAM_SET_NULL);
        iam_logger_putf(id, IAM_WARN, "Ignored value."
            "The reference to the \"%s\" %s is not set. " 
            "The NULL value is available only for arrays.",
                name, c->name);
        return NULL;
    }      
    IAM_VFF_RESET(c, last, reg);  
    v = IAM_NEW(variable);
   
    if (v == NULL) {
        c->status.init = IAM_OUT_OF_MEMORY;
        return NULL;
    }
    v->id = id;
    v->type = type;
    v->name = name;
    v->desc = desc;
    v->size = size;
    v->num.is_spec_range = false;
    v->num.int_range.min = 0;
    v->num.int_range.max = 0;
    if (ref == NULL) {
        ref = iam__malloc(size * max);
        if (ref == NULL) {
            c->status.init = IAM_OUT_OF_MEMORY;
            return NULL;
        }
        v->max = 0;
        v->is_resize = true;
    } else {
        v->max = max;
        v->is_resize = false;
    }
    v->count = v->max == 1;
    return v;
}

iam_variable_status iam__variable_warn_if_l0(iam_class_t *c, iam_variable_t *v,
    char *val) {
    iam_variable_status s = IAM_SET_NEGATIVE;
    IAM__SET_STATUS(set, s);
    iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
        "The %s \"%s\" cannot be negative (set %s).", c->name, v->name, val);
    return s;
}

#define IAM__VAR_IF_LESS_0(type, T, spec)           \
    iam_variable_status iam_variable_##type##_if_l0 \
        (iam_class_t *c, iam_variable_t *v, T val) {\
        char buf[IAM__BS];                          \
        sprintf(buf, spec, val);                    \
        return iam__variable_warn_if_l0(c, v, buf); \
    }
IAM__VAR_IF_LESS_0(int, int64_t, "%"PRId64)
IAM__VAR_IF_LESS_0(uint, uint64_t, "%"PRIu64)
IAM__VAR_IF_LESS_0(real, double, IAM_DOUBLE_SPEC)

iam_variable_status iam__variable_warn_if_oor(iam_class_t *c, iam_variable_t *v,
    char *val, char *min, char *max) {
    iam_variable_status s = IAM_OUT_OF_RANGE;
    IAM__SET_STATUS(set, s);
    iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
        "The value for %s \"%s\" does not match the range [%s, %s] (set %s).",
        c->name, v->name, min, max, val);
    return s;
}

#define IAM__VAR_IF_OUT_OF_RANGE(type, T, spec)     \
    iam_variable_status iam_variable_##type##_if_oor\
        (iam_class_t *c, iam_variable_t *v, T val) {\
        char buf[3][IAM__BS];                       \
        T min = v->num.type##_range.min;            \
        T max = v->num.type##_range.max;            \
        sprintf(buf[0], spec, val);                 \
        sprintf(buf[1], spec, min);                 \
        sprintf(buf[2], spec, max);                 \
        return iam__variable_warn_if_oor(c, v,      \
            buf[0], buf[1], buf[2]);                \
    }
IAM__VAR_IF_OUT_OF_RANGE(int, int64_t, "%"PRId64)
IAM__VAR_IF_OUT_OF_RANGE(uint, uint64_t, "%"PRIu64)
IAM__VAR_IF_OUT_OF_RANGE(real, double, IAM_DOUBLE_SPEC)

#define IAM__VAR_SET_RANGE(type, T)                         \
    iam_variable_status iam_variable_set_range_##type       \
    (iam_class_t *c, iam_variable_t *v, T min, T max) {     \
    if (max < min) { T tmp = min; min = max; max = tmp; }   \
    v->num.type##_range.min = min;                          \
    v->num.type##_range.max = max;                          \
    v->num.is_spec_range = true;                            \
    return c->status.last.set;                              \
}
IAM__VAR_SET_RANGE(int, int64_t)
IAM__VAR_SET_RANGE(uint, uint64_t)
IAM__VAR_SET_RANGE(real, double)
iam_variable_status iam_variable_set_range_ureal(iam_class_t *c,
    iam_variable_t *v, double min, double max) {
    if (min < 0) return iam_variable_real_if_l0(c, v, min);
    if (max < 0) return iam_variable_real_if_l0(c, v, max);
    return iam_variable_set_range_real(c, v, min, max);
}

iam_variable_status iam_variable_check_str(iam_class_t *c, iam_variable_t *v,
    const char *str) {
        size_t size;
        iam_variable_status s = IAM_VALUE_IS_CORRECT;
        bool value_not_found = true;
        if (str == NULL) {
            if (!v->str.is_set_null) {
                s = IAM_SET_NULL;
                IAM__SET_STATUS(set, s);
                iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
                    "The value of %s \"%s\" cannot be set to NULL.",
                    c->name, v->name);
            }
        } else {
            if (v->str.sel_count > 0) {
                for (int i = 0; i < v->str.sel_count; i++)
                    if (strcmp(v->str.sel[i], str) == 0) {
                        value_not_found = false;
                        break;
                    }
                if (value_not_found) {
                    s = IAM_VALUE_NOT_FOUND;
                    IAM__SET_STATUS(set, s);
                    iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
                        "The string \"%s\" for %s \"%s\" was not found "
                        "in the given \"sel\".",
                        str, c->name, v->name);
                    return s;
                }
            }
            size = strlen(str) + 1;
            if (size > v->size) {
                s = IAM_OVERFLOW_VALUE;
                IAM__SET_STATUS(set, s);
                iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
                    "The string \"%s\" length with '\\0' ("PRIu64") exceeds "
                    "the allowed size ("PRIu64") for %s \"%s\".",
                    str, size, v->size, c->name, v->name);
            }
        }
        return s;
}

iam_variable_status iam_variable_set_max(iam_class_t *c, iam_variable_t *v, 
    size_t max, void **var) {
    iam_variable_status s;
    if (max == 0) {
        s = IAM_SET_MAX_ERROR;
        IAM__SET_STATUS(set, s);
        iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
            "The \"max\" value for %s \"%s\" variable must be greater than 0.",
            c->name, v->name);
        return s;
    }
    if (v->is_resize)
        *var = iam__realloc(*var, v->size * max);
    else if (max > v->max) {
        s = IAM_SET_MAX_ERROR;
        IAM__SET_STATUS(set, s);
        iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
            "The \"max\" for %s \"%s\" is considered fixed.",
            c->name, v->name);
        return s;
    }
    v->max = max;
    return IAM_VALUE_IS_CORRECT;
}

iam_variable_status iam__variable_warn_if_ie(iam_class_t *c, iam_variable_t *v,
    size_t i, char *method, char* count_s, size_t count) {
    iam_variable_status s = IAM_INDEX_ERROR;
    iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
        "Index "PRIu64" is outside the permissible range"
        "for %s \"%s\" (%s, %s = "PRIu64").",
        i, c->name, v->name, method, count_s, count);
    return s;
}

iam_variable_status iam_variable_get_i_is_correct(iam_class_t *c,
    iam_variable_t *v, size_t i, void **var) {
    iam_variable_status s = IAM_VALUE_IS_CORRECT;
    if (i >= v->count)
        s = iam__variable_warn_if_ie(c, v, i, "get", "count", v->count);
    IAM__SET_STATUS(get, s);
    return s;
}

iam_variable_status iam_variable_set_i_is_correct(iam_class_t *c,
     iam_variable_t *v, size_t i, void **var) {
    iam_variable_status s = IAM_VALUE_IS_CORRECT;
    if (i == v->max) {
        if (!v->is_resize || iam_variable_set_max(c, v, i + 1, var) != s)
            s = iam__variable_warn_if_ie(c, v, i, "set", "max count", v->max);
    }
    if (i == v->count && i < v->max) 
        v->count++;
    else if (i > v->count)
        s = iam__variable_warn_if_ie(c, v, i, "set", "count", v->count);
    IAM__SET_STATUS(set, s);
    return s;
}

void iam_variable_set_value(iam_class_t *c, iam_variable_t *v, char *method,
     const iam_type_t *rt, void *res, const iam_type_t *vt, const void *val) {
    static char zero[8];
    if (rt == vt) {
        if (rt == IAM_STR) {
            if (val == NULL)
                *(char *)res = '\0';
            else {
                strcpy((char *)res, (const char *)val);
            }
        } else {
            memcpy(res, val, rt->size); 
        }
    } else {
        switch (rt->category) {
            case IAM_BOOLEAN:
                *(bool *)res = memcmp(val, zero, vt->size) != 0;
                break;
            case IAM_INTEGER:
                if (rt->size >= vt->size ||
                    memcmp(memcpy(zero, val, rt->size), val, vt->size) == 0) {
                        memcpy(res, val, rt->size); 
                } else {
                    iam_logger_putf(v->id, IAM_WARN, "Ignored value. "
                        "Value overflow detected at %s<-%s in "
                        "%s \"%s\" (%s %s)", rt->name, vt->name,
                        c->name, v->name, method, vt->to_str(val));
                }
                break;
            case IAM_REAL:
                if (rt == IAM_FLOAT)
                    *(float *)res = *(float *)val;
                else
                    *(double *)res = *(double *)val;
        }
    }
}