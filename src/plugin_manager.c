// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "list.h"
#include "os/os.h"
#include "version.h"
#include "iam/logger.h"
#include "plugin_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	iam_id_t *info;
	iam__lib_t *lib;
} iam__plugin_t;
iam__list_t iam__plugins;

void iam__plugin_manager_load(const char* plugins_dir);
void iam__plugin_load(const char *fname);
void iam__plugins_free(void *data);

void iam__plugin_manager_init(const char* plugins_dir) {
	iam__list_init(&iam__plugins);
	iam__plugin_manager_load(plugins_dir);
}

void iam__plugin_manager_exit(void) {
	iam__list_free_act(&iam__plugins, iam__plugins_free);
}

void iam__plugin_manager_load(const char* plugins_dir) {
	const char *ext, *name;
	iam__dir_t *dir = iam__dir_open(plugins_dir);
	iam__finfo_t *f;
	char buf[512];
	if (dir == NULL) {
		iam_logger_putf(&iam__api, IAM_ERROR,
			"The directory \"%s\" was not found.", plugins_dir);
		return;
	}
	iam_logger_putf(&iam__api, IAM_TRACE,
		"Search for plugins in: \"%s\".", plugins_dir);
	f = iam__dir_findfirst(dir);	
	while (f) {
		name = iam__finfo_name(f);
		f = iam__dir_findnext(dir);
		ext = strrchr(name, '.');
		if (ext == NULL || strcmp(ext, IAM_SHARED_EXT) != 0)
			continue;
		snprintf(buf, sizeof(buf), IAM_PATH_CONCAT, plugins_dir, name);
		iam__plugin_load(buf);
	}
	iam__dir_close(dir);
	iam_logger_putf(&iam__api, IAM_TRACE,
		"Plugins loaded: %d.", iam__plugins.count);
}

void iam__plugin_load(const char *name) {
	iam_id_t *info;
	iam_init_fn init;
	iam__lib_t *lib = iam__lib_open(name);
	if (lib == NULL) {
		iam_logger_putf(&iam__api, IAM_ERROR,
			"The file \"%s\" was not found.", name);
		return;
	}
	init = (iam_init_fn)iam__lib_find(lib, "iam_plugin_init");
	if (init == NULL) {
		iam_logger_puts(&iam__api, IAM_ERROR,
			"The function \"iam_plugin_init\" was not found.");
		return;
	}
	info = init();
	if (info == NULL) {
		iam__lib_close(lib);
		return;
	}
	iam__plugin_t *p = IAM__NEW(plugin);
	p->info = info;
	p->lib = lib;
	iam__list_append(&iam__plugins, p);
	iam_logger_putf(&iam__api, IAM_TRACE,
		"Loaded: %s.", name);
}

void iam__plugins_free(void *data) {
	iam__plugin_t *p = (iam__plugin_t *)data;
	iam_exit_fn exit = (iam_exit_fn)iam__lib_find(p->lib, "iam_plugin_exit");
	exit();
	iam__lib_close(p->lib);
}