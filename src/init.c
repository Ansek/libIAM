// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <iam/init.h>
#include "algorithm_manager.h"
#include "setting_manager.h"
#include "logger_manager.h"
#include "plugin_manager.h"
#include "version.h"

iam_metadata_t iam__api_md = {
    .name = IAM_INFO_NAME,
    .version = IAM_INFO_VERSION,
    .description = IAM_INFO_DESCRIPTION,
    .author = IAM_INFO_AUTHOR
};
iam__module_t iam_api_m = { .info = &iam__api_md };
iam_id_t iam__api = (iam_id_t)&iam_api_m;

iam_init_status iam_init(void) {
    iam_init_status res;
    iam__setting_manager_init();
    iam__logger_manager_init();
    iam__algorithm_manager_init();
    res = iam__plugin_manager_init(IAM_PLUGINS_DIR);
    if (res)
        return res;
    iam__setting_manager_load();
    iam__logger_manager_flush();
    return IAM_SUCCESS_INIT;
}

void iam_exit(void) {
    iam__algorithm_manager_exit();
    iam__logger_manager_exit();
    iam__setting_manager_exit();
    iam__plugin_manager_exit();
}