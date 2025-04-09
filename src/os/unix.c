// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "os.h"

iam__dir_t *iam__dir_open(const char *name) {
    return opendir(name);
} 

iam__lib_t *iam__lib_open(const char *name) {
    return dlopen(name, RTLD_LAZY);
}

iam__finfo_t *iam__dir_findfirst(iam__dir_t *dir) {
    rewinddir(dir);
    return readdir(dir);
}

iam__finfo_t *iam__dir_findnext(iam__dir_t *dir) {
    return readdir(dir);
}

const char *iam__finfo_name(iam__finfo_t *info) {
    return info->d_name;
}

const void *iam__lib_find(iam__lib_t *lib, const char *name) {
    return dlsym(lib, name);
}

void iam__dir_close(iam__dir_t *dir) {
    if (dir)
        closedir(dir);
}

void iam__lib_close(iam__lib_t *lib) {
    if (lib)
        dlclose(lib);
}