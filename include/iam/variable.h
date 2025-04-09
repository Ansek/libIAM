// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/variable.h
    \brief Управление переменными системы (настройки и параметры).

    Содержит функции и макросы для определения методов работы с памятью, 
    предназначенной для хранения настройки или отдельного параметра алгоритма.  
*/
#ifndef __IAM_VARIABLE_H__
#define __IAM_VARIABLE_H__

#include "iam.h"
#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef IAM_FLOAT_SPEC
    #define IAM_FLOAT_SPEC "E"
#endif
#ifndef IAM_DOUBLE_SPEC
    #define IAM_DOUBLE_SPEC "lE"
#endif

/*! \brief Определяет статус обращения к значению параметра.

    Если результат методов get и set не равен #IAM_VALUE_IS_CORRECT,
    тогда сохранены предыдущие значения параметров. В случае массивов, 
    могут быть перезаписаны часть значений.
*/
typedef enum {
    IAM_VALUE_IS_CORRECT,   //!< Значение получено без проблем.
    IAM_VALUE_NOT_FOUND,    //!< Значение не найдено в поле str.select.
    IAM_SET_NULL,           //!< Установлено value==NULL при is_set_null=0.
    IAM_SET_NEGATIVE,       //!< Установлено value<0 при is_unsigned=1.
    IAM_SET_MAX_ERROR,      //!< Присвоено недопустимое значение для max
    IAM_OUT_OF_RANGE,       //!< Неверный диапазон при is_spec_range=1.
    IAM_INDEX_ERROR,        //!< Выход за пределы max при is_resize=0
    IAM_OVERFLOW_VALUE      //!< Значение могло быть записано не полностью.
} iam_variable_status;

/*! \brief Категории для группировки информации о данных.
*/
typedef enum {
    IAM_BOOLEAN,    //!< Определяет тип bool
    IAM_INTEGER,    //!< Объединяет типы intN и uintN
    IAM_REAL,       //!< Объединяет типы float и double
    IAM_STRING      //!< Определяет тип char *
} iam_type_category_t;

/*! \brief Функция для перевода содержимого указателя в строку.
*/
typedef const char *(*const iam_to_str_fn)(const void *);

/*! \brief Информация о типе данных.
*/
typedef struct {            
    const char *name;       //!< Название типа данных.
    const char *spec;       //!< Спецификатор для работы с sscanf и sprintf.
    const bool is_unsigned; //!< true - значение должно быть не меньше 0. 
    const iam_type_category_t category;   //!< Определяет группу типа данных
    iam_to_str_fn to_str;   //!< Функция для вывода значения в строку
    const uint8_t size;     //!< Размер для хранения
} iam_type_t;

IAM_API extern const iam_type_t *const IAM_STR, *const IAM_BOOL,
    *const IAM_INT8, *const IAM_INT16, *const IAM_INT32, *const IAM_INT64,
    *const IAM_UINT8, *const IAM_UINT16, *const IAM_UINT32, *const IAM_UINT64,
    *const IAM_UFLOAT, *const IAM_FLOAT, *const IAM_UDOUBLE, *const IAM_DOUBLE;

/*! \brief Задает набор переменных для сохранения статуса методов.
*/
typedef struct {
    iam_variable_status reg;    //!< Статус регистрации переменной.
    iam_variable_status get;    //!< Статус получения значения переменной.
    iam_variable_status set;    //!< Статус присвоения значения переменной.
} iam_method_status_t;

/*! \brief Информация о дочернем для #iam_variable_t классе.

    Например, #setting и #parameter.
*/
typedef struct {
    const char *const name;   //!< Имя класса данных.
    struct {
        iam_init_status init; //!< Статус инициализации модуля в целом
        iam_method_status_t last;   //!< Последний статус.
        iam_method_status_t high;   //!< Самый высокий уровень.
    } status;   //!< Статус выполненных операций с данными.
} iam_class_t; 

/*! \brief Информация о переменной.

    Если простое значение: count и max равны 1, value не может быть NULL.
    Если массив значений: count==0, в случае value==NULL, устанавливаются
    значения max=0 и is_resize=1, что говорит об отсутствии дополнительных
    ограничений на добавление нового значения (до исчерпания памяти, через
    вызов realloc), если значение value задано, то is_resize=0. 
    Поле is_unsigned заполняется в зависимости от типа переменной.
    Поле is_spec_range устанавливается в 1 после установки значений range.
    Поле select позволяет установить список допустимых значений строк.
    Поле is_set_null==1, позволяет присвоить NULL через функцию set.
*/
typedef struct {
    iam_id_t id;            //!< Идентификатор модуля
    const iam_type_t *type; //!< Идентификатор типа данных.
    const char *name;       //!< Название пункта.
    const char *desc;       //!< Краткое описание пункта.    
    size_t size;            //!< Количество байт для хранения.
    size_t count;           //!< Сколько элементов хранится, по умолчанию 0.
    size_t max;             //!< Максимальное количество элементов.
    bool is_resize;         //!< true - используется динамическая память.
    union {
        struct {
            const char **sel;   //!< Список вариантов значений.
            size_t sel_count;   //!< Количество элементов в списке вариантов.
            bool is_set_null;   //!< true - можно присвоить NULL.
        } str;
        struct {
            bool is_spec_range; //!< true - задан специфичный диапазон.
            union {
                struct {
                    int64_t min;    //!< Мин. значение числа.
                    int64_t max;    //!< Макс. значение числа.
                } int_range;
                struct {
                    uint64_t min;   //!< Мин. значение числа без знака.
                    uint64_t max;   //!< Макс. значение числа без знака.
                } uint_range;
                struct {
                    double min;     //!< Мин. значение вещественного числа.
                    double max;     //!< Макс. значение вещественного числа.
                } real_range;
            };
        } num;
    };
} iam_variable_t;

/*! Сбрасывает переменные, связанные с последней операцией над переменной.
    \param с Тип переменной, #setting и #parameter.
*/
IAM_API void iam_variable_reset_last_status(iam_class_t *c);

/*! Сбрасывает переменные, связанные с сохранением максимального статуса.
    \param с Тип переменной, #setting и #parameter.
*/
IAM_API void iam_variable_reset_high_status(iam_class_t *c);

/*! Сбрасывает все переменные состояния.
    \param с Тип переменной, #setting и #parameter.
*/
IAM_API void iam_variable_reset_status(iam_class_t *c);

// Вспомогательные функции для регистрации и проверки данных
IAM_API iam_variable_t *iam_variable_reg(iam_class_t *, iam_id_t,
     const iam_type_t *, const char *, const char *, void *, size_t, size_t);
IAM_API void iam_variable_set_value(iam_class_t *, iam_variable_t *, char *,
    const iam_type_t *, void *, const iam_type_t *, const void *);
#define IAM_VF(suffix, ...)                             \
    IAM_API iam_variable_status iam_variable_##suffix   \
    (iam_class_t *, iam_variable_t *, __VA_ARGS__)
IAM_VF(int_if_l0, int64_t);
IAM_VF(uint_if_l0, uint64_t);
IAM_VF(real_if_l0, double);
IAM_VF(int_if_oor, int64_t);
IAM_VF(uint_if_oor, uint64_t);
IAM_VF(real_if_oor, double);
IAM_VF(set_range_int, int64_t, int64_t);
IAM_VF(set_range_uint, uint64_t, uint64_t);
IAM_VF(set_range_real, double, double);
IAM_VF(set_range_ureal, double, double);
IAM_VF(check_str, const char *str);
IAM_VF(set_max, size_t , void **);
IAM_VF(get_i_is_correct, size_t, void **);
IAM_VF(set_i_is_correct, size_t, void **);
// Type
#define IAM_VT_BOOL(...) bool
#define IAM_VT_NUM(u, name) u##name##_t
#define IAM_VT_REAL(u, name) name
#define IAM_VT_STR(...) char *
// Return
#define IAM_VR_NUM(T) T
#define IAM_VR_STR(T) const T
#define IAM_VR_REG(class, ...) iam_##class##_t *
#define IAM_VR_GET(class, T, t2) IAM_VR_##t2(T)
#define IAM_VR_SET(...) iam_variable_status
#define IAM_VR_SET_RANGE(...) iam_variable_status
#define IAM_VR_SET_IS_NULL(...) void
#define IAM_VR_SET_SELECT(...) void
// Header
#define IAM_VH_VAL(class, name, ...) iam_##class##name
#define IAM_VH_ARR(class, name, suffix) iam_##class##name##suffix
// Argument
#define IAM_VA_REG(arg) arg, const char *name, const char *desc
#define IAM_VA_REG_NUM(T) T *var
#define IAM_VA_REG_STR(T) T str, size_t len
#define IAM_VA_REG_NUM_A(T) T *arr, size_t max
#define IAM_VA_REG_STR_A(T) T buf, size_t len, size_t max
#define IAM_VA_REG_VAL(CT, T, t2, arg) IAM_VA_REG(arg), IAM_VA_REG_##t2(T)
#define IAM_VA_REG_ARR(CT, T, t2, arg) IAM_VA_REG(arg), IAM_VA_REG_##t2##_A(T)
#define IAM_VA_GET_VAL(CT, ...) CT *v
#define IAM_VA_GET_ARR(CT, ...) CT *v, size_t i
#define IAM_VA_SET_VAL(CT, T, ...) CT *v, const T value
#define IAM_VA_SET_ARR(CT, T, ...) CT *v, size_t i, const T value
#define IAM_VA_SET_RANGE_VAL(CT, T, ...) CT *v, T min, T max
#define IAM_VA_SET_IS_NULL_VAL(CT, ...) CT *v, bool is_null
#define IAM_VA_SET_SELECT_VAL(CT, ...) CT *v, const char **sel, size_t sel_count
// Variable
#define IAM_VV_REG_NUM_VAL(T) size_t size = sizeof(T); size_t max = 1
#define IAM_VV_REG_STR_VAL(T) T var = str; size_t size = len; size_t max = 1
#define IAM_VV_REG_NUM_ARR(T) T *var = arr; size_t size = sizeof(T)
#define IAM_VV_REG_STR_ARR(T) T var = buf; size_t size = len
#define IAM_VV_GET_NUM *var 
#define IAM_VV_GET_STR var
#define IAM_VV_SET_NUM &value
#define IAM_VV_SET_STR value
#define IAM_VV(header, T, i, ...) T var = (T)iam_##header(v, i, __VA_ARGS__)
#define IAM_VV_VAL(T, class, suf, ...) IAM_VV(class##suf, T, 0, __VA_ARGS__)
#define IAM_VV_ARR(T, class,  suf, ...) IAM_VV(class##suf, T, i, __VA_ARGS__)
#define IAM_VV_NUM(arr, T, ...) IAM_VV##arr(T *, __VA_ARGS__)
#define IAM_VV_STR(arr, T, ...) IAM_VV##arr(T, __VA_ARGS__)
#define IAM_VV_GET(t2, t3, ...) IAM_VV_##t2(__VA_ARGS__, _get, t3)
#define IAM_VV_SET(t2, t3, ...) IAM_VV_##t2(__VA_ARGS__, _set, t3, \
    IAM_VV_SET_##t2)
// Function fragments
#define IAM_VFF_RESET(class, level, method) \
    (class)->status.level.method = IAM_VALUE_IS_CORRECT
#define IAM_VFF_IF_LESS_0(type_, class, value)  \
    if (v->info->type->is_unsigned && value < 0) \
        return iam_variable_##type_##_if_l0(&class, v->info, value)
#define IAM_VFF_IF_OUT_OF_RANGE(type, class)        \
    if (v->info->num.is_spec_range &&               \
        (value < v->info->num.type##_range.min ||   \
         value > v->info->num.type##_range.max))    \
        return iam_variable_##type##_if_oor(&class, v->info, value)
#define IAM_VFF_GET_STR(...) return var
#define IAM_VFF_GET_NUM(...) \
    iam_variable_set_value(__VA_ARGS__)
// Body
#define IAM_VB(u, type, class)              \
    IAM_VFF_IF_LESS_0(type, class, value);  \
    IAM_VFF_IF_OUT_OF_RANGE(type, class)
#define IAM_VBU(u, type, class) \
    IAM_VFF_IF_OUT_OF_RANGE(u##type, class)
#define IAM_VB_BOOL(...) /* empty */
#define IAM_VB_NUM(class, T, U) IAM_VB##U(u, int, class);
#define IAM_VB_REAL(class, T, U) IAM_VB##U( , real, class);
#define IAM_VB_STR(class, T, U)                         \
    if (iam_variable_check_str(&class, v->info, value)) \
        return class.status.last.set;
#define IAM_VB_REG(class, T, t1, t2, t3, arr, id, U)    \
    IAM_VV_REG_##t2##arr(T);                            \
    iam_variable_t *v = iam_variable_reg                \
        (&class, id, t3, name, desc, var, size, max);   \
    return iam_##class##_reg(v, var)
#define IAM_VB_GET(class, T, t1, t2, t3, arr, id, U)    \
    T value = 0;                                        \
    IAM_VV_GET(t2, t3, arr, T, class);                  \
    if (var) IAM_VFF_GET_##t2(&class, v->info, "get",   \
        t3, &value, v->info->type, var);                \
    return value
#define IAM_VB_SET(class, T, t1, t2, t3, arr, id, U)        \
    IAM_VB_##t1(class, T, U)                                \
    IAM_VV_SET(t2, t3, arr, T, class);                      \
    if (var) iam_variable_set_value(&class, v->info, "set", \
        v->info->type, var, t3, IAM_VV_SET_##t2);           \
    return class.status.last.set
#define IAM_VBSR(t, c) return iam_variable_set_range_##t(c, v->info, min, max)
#define IAM_VBSR_NUM(c) IAM_VBSR(int, c)
#define IAM_VBSR_REAL(c) IAM_VBSR(real, c)
#define IAM_VBSR_UNUM(c) IAM_VBSR(uint, c)
#define IAM_VBSR_UREAL(c) IAM_VBSR(ureal, c)
#define IAM_VB_SET_RANGE(class, T, t1, t2, t3, arr, id, U)  \
    IAM_VBSR_##U##t1(&class)
#define IAM_VB_SET_IS_NULL(class, T, t1, t2, t3, arr, id, U)\
    v->info->str.is_set_null = is_null
#define IAM_VB_SET_SELECT(class, T, t1, t2, t3, arr, id, U) \
    v->info->str.sel = sel; v->info->str.sel_count = sel_count
// Static function
#define IAM_VSF(method, name, suffix, T, t1, t2, t3, class, arg, id, U, arr)\
    static inline IAM_VR##method(class, T, t2)                              \
        IAM_VH##arr(class, name, suffix)                                    \
        (IAM_VA##method##arr(iam_##class##_t, T, t2, arg))                  \
        { IAM_VB##method(class, T, t1, t2, t3, arr, id, U); }

#define IAM_VAR_BOOL(...) /* empty */
#define IAM_VAR_NUM(t1, name, T, ...)   \
    IAM_VSF(_SET_RANGE, _set_range##name, , T, t1, __VA_ARGS__)
#define IAM_VAR_REAL(...)  \
    IAM_VAR_NUM(__VA_ARGS__)
#define IAM_VAR_STR(t1, name, T, ...)                               \
    IAM_VSF(_SET_IS_NULL, _set_str_is_null, , T, t1, __VA_ARGS__)   \
    IAM_VSF(_SET_SELECT, _set_str_select, , T, t1, __VA_ARGS__)     \

#define IAM_VAR(name, ...)                       \
    IAM_VSF(_REG, _reg##name, _arr, __VA_ARGS__) \
    IAM_VSF(_GET, _get##name, _i, __VA_ARGS__)   \
    IAM_VSF(_SET, _set##name, _i, __VA_ARGS__)   \

#define IAM_VAR_FUNC(t1, name, T, t2, t3, class, arg, id, U, arr)\
    IAM_VAR(name, T, t1, t2, IAM_##U##t3, class, arg, id, U, arr)

#define IAM_VAR_FUNC_A(t1, ...)         \
    IAM_VAR_FUNC(t1, __VA_ARGS__, _VAL) \
    IAM_VAR_FUNC(t1, __VA_ARGS__, _ARR) \
    IAM_VAR_##t1(t1, __VA_ARGS__, _VAL)

#define IAM_VAR_FUNC_1(name, t1, ...) \
    IAM_VAR_FUNC_A(t1, name, IAM_VT_##t1(, name), __VA_ARGS__, )

#define IAM_VAR_FUNC_2(name, t1, ...)                               \
    IAM_VAR_FUNC_A(t1, _##name, IAM_VT_##t1(, name), __VA_ARGS__, ) \
    IAM_VAR_FUNC_A(t1, _u##name, IAM_VT_##t1(u, name), __VA_ARGS__, U)

#define IAM_VAR_FUNC_LIST(...)                                  \
    IAM_VAR_FUNC_1(_bool,   BOOL,   NUM,    BOOL,   __VA_ARGS__)\
    IAM_VAR_FUNC_2(int8,    NUM,    NUM,    INT8,   __VA_ARGS__)\
    IAM_VAR_FUNC_2(int16,   NUM,    NUM,    INT16,  __VA_ARGS__)\
    IAM_VAR_FUNC_2(int32,   NUM,    NUM,    INT32,  __VA_ARGS__)\
    IAM_VAR_FUNC_2(int64,   NUM,    NUM,    INT64,  __VA_ARGS__)\
    IAM_VAR_FUNC_2(float,   REAL,   NUM,    FLOAT,  __VA_ARGS__)\
    IAM_VAR_FUNC_2(double,  REAL,   NUM,    DOUBLE, __VA_ARGS__)\
    IAM_VAR_FUNC_1(_str,    STR,    STR,    STR,    __VA_ARGS__)

#define IAM_VARIABLE_DECLARE_CLASS(class, ...)                              \
    IAM_API extern iam_class_t class;                                       \
    IAM_API iam_##class##_t *iam_##class##_reg(iam_variable_t *, void *);   \
    IAM_API const char *iam_##class##_to_str(iam_##class##_t *);            \
    IAM_API const char *iam_##class##_to_str_i(iam_##class##_t *, size_t);  \
    IAM_API iam_variable_status iam_##class##_set_max(iam_##class##_t *, size_t);   \
    IAM_API void *iam_##class##_get(iam_##class##_t *, size_t, const iam_type_t *); \
    IAM_API void *iam_##class##_set(iam_##class##_t *, size_t, const iam_type_t *,  \
         const void *); \
    IAM_VAR_FUNC_LIST(class, __VA_ARGS__)

#define IAM_VARIABLE_DEFINE_CLASS(class)\
    iam_class_t class = { .name=#class }

#endif