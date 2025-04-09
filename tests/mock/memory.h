// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_MEMORY_H__
#define __IAM_MEMORY_H__

#include <fff.h>

DECLARE_FAKE_VALUE_FUNC1(void *, iam__malloc, size_t);
DECLARE_FAKE_VALUE_FUNC2(void *, iam__realloc, void *, size_t);
DECLARE_FAKE_VOID_FUNC1(iam__free, void *);

#define IAM_T(type) iam_##type##_t
#define IAM__T(type) iam__##type##_t
#define IAM_NEW(type) (IAM_T(type) *)iam__malloc(sizeof(IAM_T(type)))
#define IAM__NEW(type) (IAM__T(type) *)iam__malloc(sizeof(IAM__T(type)))

#endif