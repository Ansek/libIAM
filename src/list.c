// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "list.h"

void iam__list_init(iam__list_t *list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

int iam__list_append(iam__list_t *list, void *data) {
    iam__node_t *p = IAM__NEW(node);
    if (p == NULL)
        return 1;
    p->data = data;
    p->next = NULL;
    if (list->head == NULL)
        list->head = p;
    else
        list->tail->next = p;
    list->tail = p;
    list->count++;
    return 0;
}

void iam__list_remove(iam__list_t *list, void *data) {
    iam__node_t *p, *tmp = NULL;
    if (list->head == NULL)
        return;
    if (list->head->data == data) {
        tmp = list->head;
        list->head = list->head->next;
        if (list->head == NULL)
            list->tail = NULL;
    } else {
        IAM__FOREACH(p, *list)
            if (p->next != NULL && p->next->data == data) {
                tmp = p->next;
                p->next = p->next->next;
                break;
            }
        if (list->tail == tmp)
            list->tail = p;
    }
    if (tmp) {
        iam__free(tmp);
        list->count--;
    }
}

#define IAM__FOREACH_DEL(var, tmp, list)    \
    iam__node_t *var, *tmp;                 \
    for (var = list->head,                  \
        tmp = var;                          \
        var != NULL;                        \
        var = var->next,                    \
        iam__free(tmp),                     \
        tmp = var                           \
    )

void iam__list_clear(iam__list_t *list) {
    IAM__FOREACH_DEL(p, tmp, list);
    iam__list_init(list);
}

void iam__list_clear_act(iam__list_t *list, action_fn act) {
    IAM__FOREACH_DEL(p, tmp, list)
        act(p->data);
    iam__list_init(list);
}

void iam__list_free(iam__list_t *list) {
    IAM__FOREACH_DEL(p, tmp, list)
        iam__free(p->data);
    iam__list_init(list);
}

void iam__list_free_act(iam__list_t *list, action_fn act) {
    IAM__FOREACH_DEL(p, tmp, list) {
        act(p->data);
        iam__free(p->data);
    }
    iam__list_init(list);
}