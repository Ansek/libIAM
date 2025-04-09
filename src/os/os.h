// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_OS_H__
#define __IAM_OS_H__

#include "iam/iam.h"
#ifdef _WIN32
    #include "win.h"
#else
    #include "unix.h"
#endif

iam__dir_t *iam__dir_open(const char *name);
iam__lib_t *iam__lib_open(const char *name);

iam__finfo_t *iam__dir_findfirst(iam__dir_t *dir);
iam__finfo_t *iam__dir_findnext(iam__dir_t *dir);

const char *iam__finfo_name(iam__finfo_t *info);

const void *iam__lib_find(iam__lib_t *lib, const char *name);

void iam__dir_close(iam__dir_t *dir);
void iam__lib_close(iam__lib_t *lib);

#endif