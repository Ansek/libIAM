// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#ifndef __IAM_LIST_H__
#define __IAM_LIST_H__

typedef struct iam__node_s {
    void *data;
    struct iam__node_s *next;
} iam__node_t;

typedef struct {
    iam__node_t *head;
    iam__node_t *tail;
    unsigned int count;
} iam__list_t;

typedef void (*action_fn)(void *data);

#define IAM__T(type) iam__##type##_t
#define IAM__D(type, var)(IAM__T(type) *)var->data
#define IAM__NEW(type) (IAM__T(type) *)malloc(sizeof(IAM__T(type)))

#define IAM__FOREACH(var, list) for (   \
    var = (list).head;                  \
    var != NULL;                        \
    var = var->next                     \
)

void iam__list_init(iam__list_t *list);
void iam__list_append(iam__list_t *list, void *data);
void iam__list_remove(iam__list_t *list, void *data);
void iam__list_clear(iam__list_t *list);
void iam__list_free(iam__list_t *list);
void iam__list_free_act(iam__list_t *list, action_fn act);

#endif