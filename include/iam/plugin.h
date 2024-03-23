// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/plugin.h
    \brief Инициализация плагинов в libIAM.

     Для регистрации плагина требуется объявить переменную структуры
     #iam_plugin_t info, чтобы libIAM мог извлечь информацию о плагине.
     Помимо этого, нужно объявить функцию #iam_plugin_init, внутри которой 
     следуют зарегистрировать пользовательские функции. Объявление функции
     #iam_plugin_exit не является обязательной.
     
*/
#ifndef __IAM_PLUGIN_H__
#define __IAM_PLUGIN_H__

#include "iam.h"
#include <stddef.h>

/* Структура и функции для регистрации плагина */

/*! Инициализирует ресурсы плагина и их регистрации в libIAM.
    \param handle Идентификатор плагина.
    \return 0 - инициализация плагина прошло успешно.
*/
iam_id_t *iam_plugin_init(void);

/*! Освобождает ресурсы плагина
*/
void iam_plugin_exit();

/* Функции для взаимодействия с libIAM */

/*! Обработчик для двоичных данных.
    \param handle Идентификатор плагина.
    \param data Набор байт.
    \param size Количество байт.
*/
typedef void (*iam_hook_binary_fn)
     (iam_id_t *handle, const char* data, size_t size);

/*! Обработчик для k-мерного вещественного вектора.
    \param handle Идентификатор плагина.
    \param vector Вектор данных.
    \param k Количество измерений вектора.
*/
typedef void (*iam_hook_real_fn)
     (iam_id_t *handle, const float* vector, size_t k);

/*! Регистрирует обратный вызов для алгоритмов с двоичным кодированием.
    \param handle Идентификатор плагина.
    \param fn Функция вызова.
*/
extern int iam_plugin_register_balg(iam_id_t *handle, iam_hook_binary_fn fn);

/*! Регистрирует обратный вызов для алгоритмов с вещественным кодированием.
    \param handle Идентификатор плагина.
    \param fn Функция вызова.
*/
extern int iam_plugin_register_ralg(iam_id_t *handle, iam_hook_real_fn fn);

#endif