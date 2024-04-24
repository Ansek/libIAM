// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_ALGORITHM_MANAGER_H__
#define __IAM_ALGORITHM_MANAGER_H__

#include <iam/algorithm.h>
#include <common.h>

typedef struct {
    iam_binary_alg_t binary;
    iam_id_t *handle;
    iam__list_t params;
} iam__binary_alg_t;

typedef struct {
    iam_real_alg_t real;
    iam_id_t *handle;
    iam__list_t params;
} iam__real_alg_t;

void iam__algorithm_manager_init(void);
void iam__algorithm_manager_exit(void);

#endif