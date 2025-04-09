// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_SETTING_MANAGER_H__
#define __IAM_SETTING_MANAGER_H__

#include <iam/setting.h>
#include <common.h>

void iam__setting_manager_init(void);
void iam__setting_manager_exit(void);
void iam__setting_manager_load(void);

#endif