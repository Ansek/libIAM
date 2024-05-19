// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_WIN_H__
#define __IAM_WIN_H__

#include <windows.h>

typedef struct HINSTANCE__ iam__lib_t;
typedef struct {
    HANDLE dir;
    WIN32_FIND_DATA data;
	const char *path;
} iam__dir_t;
typedef WIN32_FIND_DATA iam__finfo_t;

#endif