// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/setting.h
    \brief Управление настройками системы.

    Обеспечивает локальное хранение данных и управление плагинами-хранилищами.
    Записанные значения определяются как значения по умолчанию.  
*/
#ifndef __IAM_SETTING_H__
#define __IAM_SETTING_H__

#include "variable.h"
#define IAM_T(type) iam_##type##_t

/*! \brief Информация о пункте настройки (расширение #iam_variable_t).
*/
typedef struct {
    iam_variable_t *info;   //!< Служебная информация о переменной
    void *setting;          //!< Указатель на значение.
} iam_setting_t;

typedef void (*iam_setting_save_fn)(iam_id_t id, iam_id_t module);
typedef void (*iam_setting_load_fn)(iam_id_t id, iam_id_t module);
typedef void (*iam_setting_dump_fn)(iam_id_t id);

typedef struct {
    iam_id_t id;
    iam_setting_save_fn save;
    iam_setting_load_fn load;
    iam_setting_dump_fn dump;
} iam_setting_store_t;

/*! Сбрасывает указатель к первому пункту настройки.
    \param id Идентификатор модуля.
*/
IAM_API void iam_setting_rewind(iam_id_t id);

/*! Считывает пункт настройки.
    \param id Идентификатор модуля.
    \return Следующий пункт настройки.
*/
IAM_API iam_setting_t *iam_setting_read(iam_id_t id);

/*! Регистрирует функцию обратного вызова после обновления настроек.
    \param id Идентификатор модуля.
    \param fn Функция обратного вызова.
*/
IAM_API void iam_setting_reg_callback(iam_id_t id, iam_callback_fn fn);

/*! Возвращает идентификатор для регистрации компонентов хранилища.
    \param id Идентификатор модуля.
    \return Идентификатор для хранилища.
*/
IAM_API iam_setting_store_t *iam_setting_reg_store(iam_id_t id);

/*! Регистрирует функцию для сохранения настроек модуля.
    \param store Идентификатор хранилища.
    \param fn Функция сохранения настроек.
*/
IAM_API void iam_setting_store_reg_save(iam_setting_store_t *store,
    iam_setting_save_fn fn);

/*! Регистрирует функцию для загрузки настроек модуля.
    \param store Идентификатор хранилища.
    \param fn Функция загрузки настроек.
*/
IAM_API void iam_setting_store_reg_load(iam_setting_store_t *store,
    iam_setting_load_fn fn);

/*! Регистрирует функцию для сброса накопленных данных в хранилище.
    \param store Идентификатор хранилища.
    \param fn Функция для сброса изменённых настроек в хранилище.
*/
IAM_API void iam_setting_store_reg_dump(iam_setting_store_t *store,
    iam_setting_dump_fn fn);

/*! Далее представлены обобщенные функции для работы с настройками. 
    В большинстве случаев, <T> совпадает с <type>, однако для 
    <T>==(const char *) будет <type>==(str), также дополнительно
    для вещественных чисел определены "беззнаковые" функции, т. е.
    <type>==(ufloat|udouble) с <T>==(float|double) будут запрещать 
    устанавливать значения ниже 0.
*/

/*! \fn iam_setting_t *iam_setting_reg_<type>(iam_id_t id,
        const char *name, const char *desc, <T> *ref)
    \brief Регистрирует параметр настройки для модуля.
    \param id Идентификатор модуля.
    \param name Название параметра.
    \param desc Описание параметра.
    \param ref Указатель на тип для заполнения и указания значения по умолчанию.
    \return Объект #iam_setting_t для дополнительной настройки. 
*/

/*! \fn iam_setting_t iam_setting_reg_<type>_arr(iam_id_t id,
        const char *name, const char *desc, <T> *ref, size_t count)
    \brief Регистрирует параметр настройки, представленного в виде списка.
    \param id Идентификатор модуля.
    \param name Название параметра.
    \param desc Описание параметра.
    \param arr Указатель на начало массива.
    \param count Размер массива.
    \return Объект #iam_setting_t для дополнительной настройки. 
*/

/*! \fn iam_setting_t *iam_setting_reg_str(iam_id_t id, const char *name,
        const char *desc, char *str, size_t len)
    \brief Регистрирует строковый параметр настройки.
    \param id Идентификатор модуля.
    \param name Название параметра.
    \param desc Описание параметра.
    \param str Указатель на начало строки.
    \param len Максимальный размер строки с учётом нуль-терминала.
    \return Объект #iam_setting_t для дополнительной настройки. 
*/

/*! \fn iam_setting_t *iam_setting_reg_str_arr(iam_id_t id, const char *name, 
        const char *desc, char *buf, size_t len, size_t max)
    \brief Регистрирует список строковых параметров настройки.
    \param id Идентификатор модуля.
    \param name Название параметра.
    \param desc Описание параметра.
    \param buf Указатель на начало буфера, который будет делится на строки.
    \param len Размер фрагмента для хранения одной строки с нуль-терминалом.
    \param count Количество строк.
    \return Объект #iam_setting_t для дополнительной настройки. 
*/

/*! \fn <T> iam_setting_get_<type>(iam_setting_t *v)
    \fn <T> iam_setting_get_<type>_i(iam_setting_t *v, size_t i)
    \brief Получает значение настройки.
    \param v Идентификатор настройки.
    \param i Индекс массива.
    \return Хранимое значение.
*/

/*! \fn iam_variable_status iam_setting_set_<type>(iam_setting_t *v,
         <T> value)
    \fn <T> iam_variable_status iam_setting_set_<type>_i(iam_setting_t *v,
         size_t i, <T> value)
    \brief Устанавливает значение для настройки.
    \param v Идентификатор настройки.
    \param i Индекс массива.
    \param value Записываемое значение.
    \return Статус выполнения операции (IAM_VALUE_IS_CORRECT - проблем не было).
*/

/*! \fn iam_variable_status iam_setting_set_range_<type>(iam_setting_t *v,
        <T> min, <T> max)
    \brief Устанавливает ограничение [min;max] на значение.
    <type>!=(bool|str)
    \param v Идентификатор настройки.
    \param min Минимальное значение.
    \param max Максимальное значение.
    \return Статус выполнения операции (IAM_VALUE_IS_CORRECT - проблем не было).
*/
    
/*! \fn iam_variable_status iam_setting_set_max(iam_setting_t *v, size_t max);
    \brief Изменяет размер массива, в случае если is_resize==1, иначе значение
    можно только уменьшить, без возможности возврата.
    \param v Идентификатор настройки.
    \param max Максимальное количество элементов в массиве.
    \return Статус выполнения операции (IAM_VALUE_IS_CORRECT - проблем не было).
*/

/*! \fn void iam_setting_set_str_is_null(iam_setting_t *v, bool is_null)
    \brief Регулирует установку "NULL" значений в буфер строки, при этом
    возвращаемое значение будет равно пустой строке ("").
    \param v Идентификатор настройки.
    \param is_null == true - разрешает установку NULL.
*/

/*! \fn void iam_setting_set_str_select(iam_setting_t *v, const char **sel,
        size_t sel_count)
    \brief Задаёт список значений, которые может принимать настройка.
    \param v Идентификатор настройки.
    \param sel Указатель на массив указателей на строки.
    \param sel_count Количество слов в списке.
*/

/*! \fn const char *iam_setting_to_str(iam_setting_t *v, size_t i)
        const char *iam_setting_to_str_i(iam_setting_t *v, size_t i)
    \brief Возвращает временную ссылку на значение в виде строки для записи 
        в другой буфер или вывод через printf.
    \param v Идентификатор настройки.
    \param i Индекс массива.
    \return Временная ссылка на строку с записанным значением.
*/

IAM_VARIABLE_DECLARE_CLASS(setting, iam_id_t id, id)

#endif