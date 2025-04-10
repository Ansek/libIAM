// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "plugin_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

iam__list_t iam__plugins;
void iam__plugins_free(void *data);

iam__list_t iam__libraries;
void iam__libraries_free(void *data);

iam_init_status iam__plugin_manager_search(const char *plugins_dir);
iam_init_status iam__plugin_manager_load(const char *fname);

iam_init_status iam__plugin_manager_init(const char *plugins_dir) {
	iam_init_status res;
	iam__list_init(&iam__plugins);
	iam__list_init(&iam__libraries);
	res = iam__plugin_manager_search(plugins_dir);
	return res;
}

void iam__plugin_manager_exit(void) {
	iam__list_free_act(&iam__plugins, iam__plugins_free);
	iam__list_clear_act(&iam__libraries, iam__libraries_free);
}

iam__module_t *iam__plugin_register(iam_metadata_t *info, iam_exit_fn exit) {
	int res;
	iam__module_t *plugin;
	if (info == NULL)
		return NULL;
	plugin = IAM__NEW(module);
	if (plugin == NULL)
		return NULL;
	plugin->info = info;
	plugin->current_setting = NULL;
	iam__list_init(&plugin->settings);
	plugin->exit = exit;
	plugin->setting_cb = NULL;
    res = iam__list_append(&iam__plugins, plugin);
	if (res == 1)
		return NULL;
	iam_logger_putf((iam_id_t)plugin, IAM_TRACE,
		"Registered a plugin \"%s\" Ver. %s", info->name, info->version);
	return plugin;
}

iam_init_status iam__plugin_manager_search(const char *plugins_dir) {
	iam_init_status res;
	const char *ext, *name;
	iam__dir_t *dir = iam__dir_open(plugins_dir);
	iam__finfo_t *f;
	char buf[512];
	if (dir == NULL) {
		iam_logger_putf(iam__api, IAM_ERROR,
			"The directory \"%s\" was not found.", plugins_dir);
		return IAM_PLUGIN_DIR_NOT_FOUND;
	}
	iam_logger_putf(iam__api, IAM_TRACE,
		"Search for plugins in: \"%s\".", plugins_dir);
	f = iam__dir_findfirst(dir);
	while (f) {
		name = iam__finfo_name(f);
		f = iam__dir_findnext(dir);
		ext = strrchr(name, '.');
		if (ext == NULL || strcmp(ext, IAM_SHARED_EXT) != 0)
			continue;
		snprintf(buf, sizeof(buf), IAM_PATH_CONCAT, plugins_dir, name);
		res = iam__plugin_manager_load(buf);
		if (res) {
			iam__dir_close(dir);
			return res;
		}
	}
	iam__dir_close(dir);
	iam_logger_putf(iam__api, IAM_TRACE,
		"Plugins loaded: %d.", iam__plugins.count);
	return IAM_SUCCESS_INIT;
}

iam_init_status iam__plugin_manager_load(const char *name) {
	int res;
	iam_init_t init;
	iam_exit_fn exit = NULL;
	iam__module_t *id;
	iam_metadata_t *info;
	iam__lib_t *lib = iam__lib_open(name);
	iam_init_t (*fni)(void);
	iam_exit_fn (*fne)(void);
	if (lib == NULL) {
		iam_logger_putf(iam__api, IAM_ERROR,
			"The liblary \"%s\" could not be opened.", name);
		return IAM_PLUGIN_OPEN_ERROR;
	}	
	fni = (iam_init_t (*)(void))iam__lib_find(lib, "iam_plugin_init");
	if (fni == NULL) {
		iam__lib_close(lib);
		iam_logger_puts(iam__api, IAM_ERROR,
			"The function \"iam_plugin_init\" was not found.");
		return IAM_PLUGIN_INIT_NOT_FOUND;
	}
	init = fni();
	if (init.info == NULL || init.call == NULL) {
		iam__lib_close(lib);
		return IAM_PLUGIN_INIT_FAILED;
	}
	fne = (iam_exit_fn (*)(void))iam__lib_find(lib, "iam_plugin_exit");
	if (fne != NULL)
		exit = fne();
	id = iam__plugin_register(init.info, exit);
	res = init.call((iam_id_t)id);
	if (res != 0) {
		iam__lib_close(lib);
		return IAM_PLUGIN_INIT_FAILED;
	}
	res = iam__list_append(&iam__libraries, lib);
	if (res == 1) {
		iam__lib_close(lib);
		return IAM_OUT_OF_MEMORY;
	}
	iam_logger_putf(iam__api, IAM_TRACE,
		"Loaded: %s.", name);
	return IAM_SUCCESS_INIT;
}

void iam__plugins_free(void *data) {
	iam__module_t *p = (iam__module_t *)data;
	iam__list_free(&p->settings);
	if (p->exit != NULL)
		p->exit((iam_id_t)p);
}

void iam__libraries_free(void *data) {
	iam__lib_t *lib = (iam__lib_t *)data;
	iam__lib_close(lib);
}