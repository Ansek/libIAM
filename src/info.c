// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <iam/info.h>
#include <common.h>
#include "plugin_manager.h"

iam__node_t *iam__module_current = NULL;

void iam_module_rewind(void) {
    iam__module_current = iam__plugins.head;   
}

iam_id_t iam_module_read(void) {
    iam__module_t *module;
    if (iam__module_current == NULL)
        return NULL;
    module = (iam__module_t *)iam__module_current->data;
    iam__module_current = iam__module_current->next;
    return (iam_id_t)module;
}
