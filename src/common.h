// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_COMMON_H__
#define __IAM_COMMON_H__

#include <iam/iam.h>
#include <iam/logger.h>
#include <list.h>

extern iam_id_t iam__api;

typedef struct {
    const iam_metadata_t *info;
    iam__node_t *current_setting;
    iam__list_t settings;
    iam_exit_fn exit;
    iam_callback_fn setting_cb;
} iam__module_t;

#define IAM__ID(type, var) ((iam_id_t)((iam__##type##_t*)var)->id)

#endif