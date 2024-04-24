// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/plugin.h
    \brief Инициализация плагинов в libIAM.

    Для регистрации плагина требуется:
    - реализовать функцию вида int init();
    - для регистрации динамических библиотек имя функции передаётся в макрос
    IAM_PLUGIN_DYNAMIC_INIT(init) который создаёт новую функцию с именем 
    iam_plugin_init, которую будет искать менеджер libIAM;
    - для регистрации статических библиотек нужно создать отдельный заголовочный 
    файл для подключения во внешнюю программу и передать имя в функцию
    #iam_register_init (iam/init.h);
    - после этого надо вызвать функцию #iam_plugin_register передав в неё
    значение структуры #iam_metadata_t, чтобы получить идентификатор текущего
    плагина, который используется в вызовах остальных модулей системы.

    Функция void exit() является необязательной и служит для освобождения 
    внутренних ресурсов плагина. Для регистрации существуют аналогичные функции
    IAM_PLUGIN_DYNAMIC_EXIT и iam_register_exit.     
*/
#ifndef __IAM_PLUGIN_H__
#define __IAM_PLUGIN_H__

#include "iam.h"

#ifdef IAM_STATIC_PLUGIN
    #define IAM_PLUGIN_DYNAMIC_INIT(fn) /* empty */
    #define IAM_PLUGIN_DYNAMIC_EXIT(fn) /* empty */
#else
    #define IAM_PLUGIN_DYNAMIC_INIT(fn) \
        IAM_PLUGIN int iam_plugin_init(void) { return fn(); }
    #define IAM_PLUGIN_DYNAMIC_EXIT(fn) \
        IAM_PLUGIN void iam_plugin_exit(void) { fn(); }
#endif

IAM_API iam_id_t *iam_plugin_register(iam_metadata_t *info);

#endif