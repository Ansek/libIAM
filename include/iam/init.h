// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

/*! \file iam/init.h
    \brief Инициализации модулей libIAM.

    Работа libIAM начинается с вызова #iam_init() и завершается при вызове
    #iam_exit().     
*/
#ifndef __IAM_INIT_H__
#define __IAM_INIT_H__

/*! Запускает модули libIAM.
    \return 0 - все модули libIAM успешно запущены.
*/
int iam_init();

/*! Освобождает все ресурсы связанные с libIAM.
*/
void iam_exit();

#endif