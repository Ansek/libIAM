// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "os.h"

TCHAR lpBuffer[MAX_PATH];

iam__dir_t *iam__dir_open(const char *name) {
    iam__dir_t *dir;
	DWORD test = SearchPath(NULL, TEXT(name), NULL, MAX_PATH, lpBuffer, NULL);
    if (test == 0)   
        return NULL;
    dir = (iam__dir_t *)malloc(sizeof(iam__dir_t));
    dir->dir = NULL;
	dir->path = name;
    return dir;
}

iam__lib_t *iam__lib_open(const char *name) {
    return LoadLibrary(TEXT(name));
}

iam__finfo_t *iam__dir_findfirst(iam__dir_t *dir) {
    char* path;
    size_t len;
    if (dir->dir != NULL)
        FindClose(dir->dir);
    len = strlen(dir->path) + 5;
    path = (char *)malloc(len);
    strcpy(path, dir->path);
    strcat(path, "\\*.*");
    dir->dir = FindFirstFile(path, &dir->data);
    free(path);
    return &dir->data;
}

iam__finfo_t *iam__dir_findnext(iam__dir_t *dir) {
	if (FindNextFile(dir->dir, &dir->data))
		return &dir->data;
	else
		return NULL;
}

const char *iam__finfo_name(iam__finfo_t *info) {
    return info->cFileName;
}

const void *iam__lib_find(iam__lib_t *lib, const char *name) {
    return GetProcAddress(lib, name);
}

void iam__dir_close(iam__dir_t *dir) {
    if (dir->dir != NULL)
        FindClose(dir->dir);
    free(dir);
}

void iam__lib_close(iam__lib_t *lib) {
    FreeLibrary(lib);
}