// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_OS_H__
#define __IAM_OS_H__

#include <iam/iam.h>
#include <fff.h>

typedef void iam__lib_t;
typedef void iam__dir_t;
typedef void iam__finfo_t;

DECLARE_FAKE_VALUE_FUNC1(iam__dir_t *, iam__dir_open, const char *);
DECLARE_FAKE_VALUE_FUNC1(iam__lib_t *, iam__lib_open, const char *);
DECLARE_FAKE_VALUE_FUNC1(iam__finfo_t *, iam__dir_findfirst, iam__dir_t *);
DECLARE_FAKE_VALUE_FUNC1(iam__finfo_t *, iam__dir_findnext, iam__dir_t *);
DECLARE_FAKE_VALUE_FUNC1(const char *, iam__finfo_name, iam__finfo_t *);
DECLARE_FAKE_VALUE_FUNC2(void *, iam__lib_find, iam__lib_t *, const char *);
DECLARE_FAKE_VOID_FUNC1(iam__dir_close, iam__dir_t *);
DECLARE_FAKE_VOID_FUNC1(iam__lib_close, iam__lib_t *);

#endif