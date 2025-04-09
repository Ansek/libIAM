// Copyright (c) 2024 Alexander Sekunov
// License: http://opensource.org/licenses/MIT

#include <iam/plugin.h>
#include <iam/logger.h>
#include <iam/setting.h>
#include <stdio.h>

#define FILENAME "log.txt"
FILE *f;

static iam_metadata_t info = {
    .name = "log_txt",
    .version = "0.1",
    .description = "Saving logs in a txt file",
    .author = "Alexander Sekunov"
};

static void save(iam_log_t *log) {
    fputs(log->msg, f);
    fputc('\n', f);
}

iam_setting_t *fn;
char filename[255] = FILENAME;

static void load_setting(iam_id_t id) {
    f = fopen(filename, "a");
    if (f == NULL)
        IAM_LOG_ERR("Failed to open the file \"s\".", filename);
}

int log_txt_init(iam_id_t id) {
    fn = iam_setting_reg_str(id, "logfile", "Log file name", filename, 255);
    iam_setting_reg_callback(id, load_setting);
    iam_logger_reg_save(id, IAM_ALL, save);
    return 0;
}

void log_txt_exit(iam_id_t id) {
    fclose(f);
}

IAM_PLUGIN_DYNAMIC_INIT(info, log_txt_init);
IAM_PLUGIN_DYNAMIC_EXIT(log_txt_exit);