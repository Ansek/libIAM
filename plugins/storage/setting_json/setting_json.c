// Copyright (c) 2024 Alexander Sekunov
// License: http://opensource.org/licenses/MIT

#include <iam/plugin.h>
#include <iam/logger.h>
#include <iam/setting.h>
#include <stdio.h>
#include <jansson.h>

#define FILENAME "setting.json"
json_t *root = NULL;
bool has_dump = false;

static iam_metadata_t info = {
    .name = "setting_json",
    .version = "0.1",
    .description = "Saving settings in a json file",
    .author = "Alexander Sekunov"
};

typedef void (*json_action_fn)(iam_id_t module, json_t *data);
static void dump(iam_id_t id);

static void copy_value(iam_setting_t *s, size_t  i, json_t *stg) {
    switch (s->info->type->category) {
        case IAM_BOOLEAN:
            iam_setting_set_bool_i(s, i, json_integer_value(stg));
            break;
        case IAM_INTEGER:
            iam_setting_set_int64_i(s, i, json_integer_value(stg));
            break;
        case IAM_REAL:
            iam_setting_set_double_i(s, i, json_real_value(stg));
            break;
        case IAM_STRING:
            iam_setting_set_str_i(s, i, json_string_value(stg));
    }
}

static void load_from_json(iam_id_t module, json_t *data) {
    size_t i;
    json_t *stg;
    iam_setting_t *s;
    iam_setting_rewind(module);
    while (s = iam_setting_read(module)) {
        stg = json_object_get(data, s->info->name);
        if (stg == NULL) {
            has_dump = true;
            continue;
        }
        if (json_is_array(stg)) {
            data = stg;
            json_array_foreach(data, i, stg)
                copy_value(s, i, stg);
        } else
            copy_value(s, 0, stg);
    }
}

static void save_to_json(iam_id_t module, json_t *data) {
    size_t i;
    iam_setting_t *s;
    iam_setting_rewind(module);
    json_object_clear(data);
    while (s = iam_setting_read(module)) {
        json_t *stg;
        if (s->info->max != 1) {
            stg = json_array();
            json_object_set(data, s->info->name, stg);
            data = stg;
        }
        for (i = 0; i < s->info->count; i++) {
            switch (s->info->type->category) {
                case IAM_BOOLEAN:
                case IAM_INTEGER:
                    stg = json_integer(iam_setting_get_int64_i(s, i));
                    break;
                case IAM_REAL:
                    stg = json_real(iam_setting_get_double_i(s, i));
                    break;
                case IAM_STRING:
                    stg = json_string(iam_setting_get_str_i(s, i));
            }
            if (s->info->max == 1)
                json_object_set(data, s->info->name, stg);
            else
                json_array_append(data, stg);
        }
    }
    has_dump = true;
}

static void find_and_exec(iam_id_t id, iam_id_t module, json_action_fn act) {
    const char *key;
    json_t *data;
    json_object_foreach(root, key, data)
        if (strcmp(key, module->info->name) == 0) {
            act(module, data);
            return;
        }
    data = json_object();
    json_object_set(root, module->info->name, data);
    save_to_json(module, data);
}

static void save(iam_id_t id, iam_id_t module) {
    find_and_exec(id, module, save_to_json);
}

static void load(iam_id_t id, iam_id_t module) {
    find_and_exec(id, module, load_from_json);
}

static void dump(iam_id_t id) {
    if (has_dump) {
        FILE *f = fopen(FILENAME, "w");
        if (f == NULL) {
            IAM_LOG_ERR("Failed to open the file \"s\".", FILENAME);
            return;
        }
        json_dumpf(root, f, 0);
        fclose(f);
        has_dump = false;
    }
}

int setting_init(iam_id_t id) {
    json_error_t err;
    FILE *f = fopen(FILENAME, "r");
    if (f != NULL) {
        root = json_loadf(f, 0, &err);  
        fclose(f);
    }
    if (root == NULL)
        root = json_object();
    iam_setting_store_t *store = iam_setting_reg_store(id);
    iam_setting_store_reg_save(store, save);
    iam_setting_store_reg_load(store, load);
    iam_setting_store_reg_dump(store, dump);
    return 0;
}

void setting_exit(iam_id_t id) {
    dump(id);
}

IAM_PLUGIN_DYNAMIC_INIT(info, setting_init);
IAM_PLUGIN_DYNAMIC_EXIT(setting_exit);