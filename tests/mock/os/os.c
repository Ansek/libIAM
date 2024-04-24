// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "os.h"

DEFINE_FAKE_VALUE_FUNC1(iam__dir_t *, iam__dir_open, const char *);
DEFINE_FAKE_VALUE_FUNC1(iam__lib_t *, iam__lib_open, const char *);
DEFINE_FAKE_VALUE_FUNC1(iam__finfo_t *, iam__dir_findfirst, iam__dir_t *);
DEFINE_FAKE_VALUE_FUNC1(iam__finfo_t *, iam__dir_findnext, iam__dir_t *);
DEFINE_FAKE_VALUE_FUNC1(const char *, iam__finfo_name, iam__finfo_t *);
DEFINE_FAKE_VALUE_FUNC2(void *, iam__lib_find, iam__lib_t *, const char *);
DEFINE_FAKE_VOID_FUNC1(iam__dir_close, iam__dir_t *);
DEFINE_FAKE_VOID_FUNC1(iam__lib_close, iam__lib_t *);