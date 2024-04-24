// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/setting.h
    \brief Управление настройками системы.

    Обеспечивает локальное хранение данных и управление плагинами-хранилищами.
    Записанные значения определяются как значения по умолчанию.  
*/
#ifndef __IAM_SETTING_H__
#define __IAM_SETTING_H__

#include "iam.h"
#include <stdint.h>
#include <stddef.h>

/*! \brief Информация о типе данных для записи значения настройки.

    Представляет 8-битное число, которое закодировано
    по правилу (от младшему к старшего биту, если установлена 1):
    - 0 бит:  значение интерпретируется как строка;
    - 1-2 бит: определяет тип вещественного числа;
    - 3-6 бит: определяет размерность целого числа (8, 16, 32, 64);
    - 7 бит: значение может быть отрицательным.
*/
typedef enum {
    IAM_STRING  = 0b00000001,
    IAM_UFLOAT  = 0b00000010,
    IAM_FLOAT   = 0b10000010,
    IAM_UDOUBLE = 0b00000100,
    IAM_DOUBLE  = 0b10000100,
    IAM_UINT8   = 0b00001000,
    IAM_INT8    = 0b10001000,
    IAM_UINT16  = 0b00010000,
    IAM_INT16   = 0b10010000,
    IAM_UINT32  = 0b00100000,
    IAM_INT32   = 0b10100000,
    IAM_UINT64  = 0b01000000,
    IAM_INT64   = 0b11000000
} iam_setting_type_t;

/*! \brief Информация о пункте настройки.

    По умолчанию max_size = 1. При max_size > 1: речь идёт о массиве.
*/
typedef struct {
    const char *name;         //!< Название пункта.
    iam_setting_type_t type;  //!< Тип данных
    union {
        size_t size;          //!< Максимальный размер массива
        struct {
            uint32_t count;   //!< Максимальное количество строк
            uint32_t length;  //!< Максимальная длина строк
        } str;
    } max;
} iam_setting_t;

typedef void (*iam_setting_save_fn)(iam_id_t *handle);
typedef void (*iam_setting_load_fn)(iam_id_t *handle);

typedef struct {
    iam_setting_save_fn save;
    iam_setting_load_fn load;
} iam_setting_store_t;

/*! Сбрасывает указатель и возвращает первый пункт настройки.
    \param handle Идентификатор модуля.
*/
IAM_API void iam_setting_rewind(iam_id_t *handle);

/*! Считывает пункт настройки.
    \param handle Идентификатор модуля.
    \return Следующий пункт настройки.
*/
IAM_API iam_setting_t *iam_setting_read(iam_id_t *handle);

/*! Возвращает идентификатор для регистрации компонентов хранилища.
    \param handle Идентификатор модуля.
    \return Идентификатор для хранилища.
*/
IAM_API iam_setting_store_t *iam_setting_reg_store(iam_id_t *handle);

/*! Регистрирует функцию генерации детекторов на основе бинарного кодирования.
    \param handle Идентификатор хранилища.
    \param fn Функция сохранения настроек.
*/
IAM_API void iam_setting_store_reg_save(iam_setting_store_t *handle,
    iam_setting_save_fn fn);

/*! Регистрирует функцию анализа на основе детекторов с бинарным кодированием.
    \param handle Идентификатор хранилища.
    \param fn Функция загрузки настроек.
*/
IAM_API void iam_setting_store_reg_load(iam_setting_store_t *handle,
    iam_setting_load_fn fn);

/*! \fn void iam_setting_reg_<type>(const char *name, <type> *ref)
    \brief Регистрирует параметр настройки для модуля.
 
    \param handle Идентификатор модуля.
    \param name Название параметра.
    \param ref Указатель на тип для заполнения и указания значения по умолчанию.
*/

IAM_API void iam_setting_reg_int8(iam_id_t *handle, const char *name, int8_t *ref);
IAM_API void iam_setting_reg_int16(iam_id_t *handle, const char *name, int16_t *ref);
IAM_API void iam_setting_reg_int32(iam_id_t *handle, const char *name, int32_t *ref);
IAM_API void iam_setting_reg_int64(iam_id_t *handle, const char *name, int64_t *ref);
IAM_API void iam_setting_reg_uint8(iam_id_t *handle, const char *name, uint8_t *ref);
IAM_API void iam_setting_reg_uint16(iam_id_t *handle, const char *name, uint16_t *ref);
IAM_API void iam_setting_reg_uint32(iam_id_t *handle, const char *name, uint32_t *ref);
IAM_API void iam_setting_reg_uint64(iam_id_t *handle, const char *name, uint64_t *ref);
IAM_API void iam_setting_reg_float(iam_id_t *handle, const char *name, float *ref);
IAM_API void iam_setting_reg_double(iam_id_t *handle, const char *name, double *ref);
IAM_API void iam_setting_reg_ufloat(iam_id_t *handle, const char *name, float *ref);
IAM_API void iam_setting_reg_udouble(iam_id_t *handle, const char *name, double *ref);

/*! \fn void iam_setting_reg_<type>_arr(iam_id_t *handle, const char *name, <type> *ref, size_t max)
    \brief Регистрирует параметр настройки, представленного в виде списка.
 
    \param handle Идентификатор модуля.
    \param name Название параметра.
    \param arr Указатель на начало массива.
    \param max Максимальный размер массива.
*/

IAM_API void iam_setting_reg_int8_arr(iam_id_t *handle, const char *name, int8_t *arr, size_t max);
IAM_API void iam_setting_reg_int16_arr(iam_id_t *handle, const char *name, int16_t *arr, size_t max);
IAM_API void iam_setting_reg_int32_arr(iam_id_t *handle, const char *name, int32_t *arr, size_t max);
IAM_API void iam_setting_reg_int64_arr(iam_id_t *handle, const char *name, int64_t *arr, size_t max);
IAM_API void iam_setting_reg_uint8_arr(iam_id_t *handle, const char *name, uint8_t *arr, size_t max);
IAM_API void iam_setting_reg_uint16_arr(iam_id_t *handle, const char *name, uint16_t *arr, size_t max);
IAM_API void iam_setting_reg_uint32_arr(iam_id_t *handle, const char *name, uint32_t *arr, size_t max);
IAM_API void iam_setting_reg_uint64_arr(iam_id_t *handle, const char *name, uint64_t *arr, size_t max);
IAM_API void iam_setting_reg_float_arr(iam_id_t *handle, const char *name, float *arr, size_t max);
IAM_API void iam_setting_reg_double_arr(iam_id_t *handle, const char *name, double *arr, size_t max);
IAM_API void iam_setting_reg_ufloat_arr(iam_id_t *handle, const char *name, float *arr, size_t max);
IAM_API void iam_setting_reg_udouble_arr(iam_id_t *handle, const char *name, double *arr, size_t max);

/*! \brief Регистрирует строковый параметр настройки.
    
    В отличие от int8 от значения max вычитается 1 для '\0'.
    \param handle Идентификатор модуля.
    \param name Название параметра.
    \param str Указатель на начало строки.
    \param max Максимальный размер строки с учётом нуль-терминала.
*/
IAM_API void iam_setting_reg_string(iam_id_t *handle, const char *name, char *str, size_t max);

/*! \brief Регистрирует список строковых параметров настройки.
    
    В отличие от int8 от значения max вычитается 1 для '\0'.
    \param handle Идентификатор модуля.
    \param name Название параметра.
    \param str Указатель на начало строки.
    \param max Максимальный размер строки с учётом нуль-терминала.
    \param count Количество строк.
*/
IAM_API void iam_setting_reg_string_arr(iam_id_t *handle, const char *name, char *str, uint32_t max, uint32_t count);

#endif