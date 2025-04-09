// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/init.h
    \brief Инициализации модулей libIAM.

    Работа libIAM начинается с вызова #iam_init() и завершается при вызове
    #iam_exit(). C помощью #iam_reg_init и #iam_reg_exit можно зарегистрировать
    функции как плагины (например, для статических библиотек).
*/
#ifndef __IAM_INIT_H__
#define __IAM_INIT_H__

#include "iam.h"

/*! Запускает модули libIAM.
    \return 0 - все модули libIAM успешно запущены.
*/
IAM_API iam_init_status iam_init(void);

/*! Освобождает все ресурсы связанные с libIAM.
*/
IAM_API void iam_exit(void);

/*! Регистрация метаданных и функции для инициализации статического плагина.
*/
IAM_API int iam_register_init(iam_metadata_t info, iam_init_fn init);

/*! Регистрация функции для освобождения ресурсов статического плагина.
*/
IAM_API void iam_register_exit(iam_exit_fn exit);

#endif