// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_PLUGIN_MANAGER_H__
#define __IAM_PLUGIN_MANAGER_H__

#include <iam/plugin.h>
#include <common.h>
#include <os/os.h>

iam_init_status_t iam__plugin_manager_init(const char* plugins_dir);
void iam__plugin_manager_exit(void);

#endif