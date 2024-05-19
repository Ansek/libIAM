// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_LIST_H__
#define __IAM_LIST_H__

#include <memory.h>

typedef struct {
    void *next;
} iam__node_t;

typedef struct {
    void **d;
    int i, count;
    iam__node_t *head, *tail;
} iam__list_t;
typedef void *action_fn;

#define IAM_D(type, var)((IAM_T(type) *)var)
#define IAM__D(type, var)((IAM__T(type) *)var)
#define IAM__FOREACH(var, list) for (   \
    list.i = 0,                         \
    var = list.d[0];                    \
    list.i < list.count;                \
    list.i++,                           \
    var = list.d[list.i]                \
)

DECLARE_FAKE_VOID_FUNC1(iam__list_init, iam__list_t *);
DECLARE_FAKE_VALUE_FUNC2(int, iam__list_append, iam__list_t *, void *);
DECLARE_FAKE_VOID_FUNC2(iam__list_remove, iam__list_t *, void *);
DECLARE_FAKE_VOID_FUNC1(iam__list_clear, iam__list_t *);

void iam__list_clear_act(iam__list_t *list, action_fn act);
void iam__list_free(iam__list_t *list);
void iam__list_free_act(iam__list_t *list, action_fn act);

#define IAM_RESET_APPEND(buf, res) do {     \
	RESET_FAKE(iam__malloc);                \
	RESET_FAKE(iam__list_append);           \
	iam__malloc_fake.return_val = buf;      \
	iam__list_append_fake.return_val = res; \
} while(0)

#endif