// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_MEMORY_H__
#define __IAM_MEMORY_H__

#include <stdlib.h>

static inline void *iam__malloc(size_t size) {
    return malloc(size);
}

static inline void iam__free(void *ptr) {
    free(ptr);
}

#define IAM__T(type) iam__##type##_t
#define IAM__NEW(type) (IAM__T(type) *)iam__malloc(sizeof(IAM__T(type)))

#endif