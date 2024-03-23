
// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/logger.h
    \brief Управление логами libIAM.

     Содержит методы для передачи сообщений в лог libIAM.     
*/
#ifndef __IAM_LOGGER_H__
#define __IAM_LOGGER_H__

#include "iam/iam.h"

/*! Определение уровней логирования.
*/
typedef enum {
    IAM_ALL     = 0xFF, //!< Вывод сообщения в любом случае.
    IAM_TRACE   = 0x01, //!< Отслеживание действия в системе.
    IAM_DEBUG   = 0x02, //!< Отладочная информация.
    IAM_INFO    = 0x04, //!< Информация о ключевом событие.
    IAM_WARN    = 0x08, //!< Предупреждение.
    IAM_ERROR   = 0x10, //!< Ошибка, не нарушающая работу системы в целом.
    IAM_FATAL   = 0x20  //!< Ошибка, после которой работа системы невозможна.
} iam_logger_level_t;

#ifndef IAM_LOG_LEVELS
    #ifdef IAM_RELEASE
        #define IAM_LOG_LEVELS ~(IAM_TRACE | IAM_DEBUG)
    #else
        #define IAM_LOG_LEVELS IAM_ALL
    #endif
#endif

#define IAM_LOG_MAX_SIZE 512

/*! Вывод простого сообщения в лог.
    \param handle Идентификатор модуля.
    \param level  Уровень сообщения.
    \param msg    Текст сообщения.    
*/
extern void iam_logger_puts(iam_id_t *handle, iam_logger_level_t level,
    const char* msg);

/*! Вывод простого сообщения в лог.
    \param handle Идентификатор модуля.
    \param level  Уровень сообщения.
    \param msg    Текст сообщения.
*/
extern void iam_logger_putf(iam_id_t *handle, iam_logger_level_t level,
    const char* msg, ...);

#endif