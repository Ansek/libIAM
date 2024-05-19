// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/algorithm.h
    \brief Представление алгоритмов в системе.

    Хранение информации об алгоритмах и их экземплярах.     
*/
#ifndef __IAM_ALGORITHM_H__
#define __IAM_ALGORITHM_H__

#include "iam.h"
#include <stddef.h>

/*! Обработчик для генерации детекторов на основе бинарного кодирования.
    \param id Идентификатор плагина.
    \param data Набор байт.
    \param size Количество байт.
*/
typedef void (*iam_binary_generate_fn)
     (iam_id_t id, const char *data, size_t size);

/*! Обработчик для анализа на основе детекторов с бинарным кодированием.
    \param id Идентификатор плагина.
    \param data Набор байт.
    \param size Количество байт.
    \return 0 - аномалий не найдено.
*/
typedef int (*iam_binary_analyze_fn)
     (iam_id_t id, const char *data, size_t size);

/*! Обработчик для генерации детекторов на основе вещественного кодирования.
    \param id Идентификатор плагина.
    \param vector Вектор данных.
    \param k Количество измерений вектора.
*/
typedef void (*iam_real_generate_fn)
     (iam_id_t id, const float *vector, size_t k);

/*! Обработчик для анализа на основе детекторов с вещественным кодированием.
    \param id Идентификатор плагина.
    \param vector Вектор данных.
    \param k Количество измерений вектора.
    \return 0 - аномалий не найдено.
*/
typedef int (*iam_real_analyze_fn)
     (iam_id_t id, const float *vector, size_t k);

typedef struct {
    iam_binary_generate_fn generate;
    iam_binary_analyze_fn analyze;
} iam_binary_alg_t;

typedef struct {
    iam_real_generate_fn generate;
    iam_real_analyze_fn analyze;
} iam_real_alg_t;

/*! Возвращает идентификатор для регистрации компонентов бинарного алгоритма.
    \param id Идентификатор плагина.
    \return Идентификатор для бинарного алгоритма.
*/
IAM_API iam_binary_alg_t *iam_algorithm_reg_binary(iam_id_t id);

/*! Возвращает идентификатор для регистрации компонентов вещественного алгоритма.
    \param id Идентификатор плагина.
    \return Идентификатор для вещественного алгоритма.
*/
IAM_API iam_real_alg_t *iam_algorithm_reg_real(iam_id_t id);

/*! Регистрирует функцию генерации детекторов на основе бинарного кодирования.
    \param alg Идентификатор алгоритма.
    \param fn Функция генерации детекторов.
*/
IAM_API void iam_binary_alg_reg_generate(iam_binary_alg_t *alg,
    iam_binary_generate_fn fn);

/*! Регистрирует функцию анализа на основе детекторов с бинарным кодированием.
    \param alg Идентификатор алгоритма.
    \param fn Функция анализа данных.
*/
IAM_API void iam_binary_alg_reg_analyze(iam_binary_alg_t *alg,
    iam_binary_analyze_fn fn);

/*! Регистрирует функцию генерации детекторов на основе вещественного кодирования.
    \param alg Идентификатор алгоритма.
    \param fn Функция генерации детекторов.
*/
IAM_API void iam_real_alg_reg_generate(iam_real_alg_t *alg,
    iam_real_generate_fn fn);

/*! Регистрирует функцию анализа на основе детекторов с вещественным кодированием.
    \param alg Идентификатор алгоритма.
    \param fn Функция анализа данных.
*/
IAM_API void iam_real_alg_reg_analyze(iam_real_alg_t *alg,
    iam_real_analyze_fn fn);

#endif