// Copyright (c) 2024 Alexander Sekunov
// License: http://opensource.org/licenses/MIT

#include <iam/plugin.h>
#include <iam/logger.h>
#include <stdio.h>

static iam_metadata_t info = {
    .name = "log_txt",
    .version = "0.1",
    .description = "Saving logs in a txt file",
    .author = "Alexander Sekunov"
};

static void save(iam_log_t *log) {
    puts(log->msg);
}

int log_txt_init(void) {
    iam_id_t *id = iam_plugin_register(&info);
    iam_logger_reg_save(id, IAM_ALL, save);
    return 0;
}

void log_txt_exit(void) {
}

IAM_PLUGIN_DYNAMIC_INIT(log_txt_init);
IAM_PLUGIN_DYNAMIC_EXIT(log_txt_exit);