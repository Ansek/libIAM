// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/info.h
    \brief Получение информации о компонентах системы.

    Содержит функции для получение информации о внешних модулях системы.  
*/
#ifndef __IAM_INFO_H__
#define __IAM_INFO_H__

#include "iam.h"

/*! Сбрасывает указатель к первому модулю системы.
*/
IAM_API void iam_module_rewind(void);

/*! Считывает информацию о модулях.
    \return Идентификатор модуля.
*/
IAM_API iam_id_t iam_module_read(void);

#endif