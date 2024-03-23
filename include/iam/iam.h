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
} iam_id_t;

typedef iam_id_t *(*iam_init_fn)(void);
typedef void (*iam_exit_fn)(void);

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
        __declspec(dllexport) iam_id_t *iam_plugin_init(void);
        __declspec(dllexport) void iam_plugin_exit(void);
    #endif
#endif

#ifndef IAM_API
    #define IAM_API /* empty */
#endif

#ifdef IAM_STATIC_PLUGIN
    #define IAM_PLUGIN_DYNAMIC_INIT(fn) /* empty */
    #define IAM_PLUGIN_DYNAMIC_EXIT(fn) /* empty */
#else
    #define IAM_PLUGIN_DYNAMIC_INIT(fn) \
        iam_id_t *iam_plugin_init(void) { return fn(); }
    #define IAM_PLUGIN_DYNAMIC_EXIT(fn) \
        void iam_plugin_exit(void) { fn(); }
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