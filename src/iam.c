// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "plugin_manager.h"
#include <stdio.h>

int iam_init(void) {
    puts("iam_init");
    iam__plugin_manager_init(IAM_PLUGINS_DIR);
}

void iam_exit(void) {
    iam__plugin_manager_exit();
    puts("iam_exit");
}