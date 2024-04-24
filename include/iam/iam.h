// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/iam.h
    \brief Корневой заголовочный файл libIAM.

    Определение общих типов данных и макросов для всех зависимых модулей.     
*/
#ifndef __IAM_H__
#define __IAM_H__

/*! \brief Определяет информацию о модуле.
*/
typedef const struct {
     const char *name;          //!< Отображаемое имя.
     const char *version;       //!< Версия.
     const char *description;   //!< Дополнительное описание.
     const char *author;        //!< Автор.
} iam_metadata_t;

/*! \brief Идентификатор модуля.
*/
typedef struct {
    iam_metadata_t *info;    
} iam_id_t;

typedef int (*iam_init_fn)(void);
typedef void (*iam_exit_fn)(void);

/*! \brief Определяет состояние после инициализации модулей библиотеки.
*/
typedef enum {
    IAM_SUCCESS_INIT,           //!< Все модули успешно инициализированы
    IAM_PLUGIN_DIR_NOT_FOUND,   //!< Не найден каталог плагинов
    IAM_PLUGIN_OPEN_ERROR,      //!< Ошибка при попытке открытия библиотеки
    IAM_PLUGIN_INIT_NOT_FOUND,  //!< Не удалось найти функцию iam_plugin_init
    IAM_PLUGIN_INIT_FAILED,     //!< Какой-то из плагинов не был инициализирован
    IAM_OUT_OF_MEMORY           //!< Не хватило памяти на инициализацию
} iam_init_status_t; 

#if defined(IAM_SHARED_API) && defined(IAM_STATIC_API)
    #error "The action is not defined (IAM_SHARED_API && IAM_STATIC_API)."
#endif

#if defined(IAM_SHARED_PLUGIN) && defined(IAM_STATIC_PLUGIN)
    #error "The action is not defined (IAM_SHARED_PLUGIN && IAM_STATIC_PLUGIN)."
#endif

#ifdef _WIN32
    #if !defined(IAM_STATIC_API)
        #if defined(IAM_SHARED_PLUGIN) || defined(IAM_STATIC_PLUGIN)
            #define IAM_API  __declspec(dllimport)
        #else
            #define IAM_API  __declspec(dllexport)
        #endif 
    #endif
    #ifdef IAM_SHARED_PLUGIN
        #define IAM_PLUGIN __declspec(dllexport)
    #endif
#endif

#ifndef IAM_API
    #define IAM_API /* empty */
#endif

#ifndef IAM_PLUGIN
    #define IAM_PLUGIN /* empty */
#endif

#ifndef IAM_PLUGINS_DIR
    #define IAM_PLUGINS_DIR "plugins"
#endif

#ifdef _WIN32
    #define IAM_PATH_CONCAT "%s\\%s"
    #define IAM_SHARED_EXT ".dll"
#else
    #define IAM_PATH_CONCAT "%s/%s"
    #define IAM_SHARED_EXT ".so"
#endif

#endif