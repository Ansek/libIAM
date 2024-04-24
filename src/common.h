// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_COMMON_H__
#define __IAM_COMMON_H__

#include <iam/iam.h>
#include <iam/logger.h>
#include <list.h>
#include "version.h"

extern iam_id_t iam__api;

typedef struct {
    iam_id_t id;
    iam__node_t *current_setting;
    iam__list_t settings;
} iam__module_t;

#define IAM__INFO(handle) (((iam__module_t *)(handle))->id.info)
#define IAM__HANDLE(type) (((iam__##type##_t*)handle)->handle)

#endif