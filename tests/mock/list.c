// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "list.h"

DEFINE_FAKE_VOID_FUNC1(iam__list_init, iam__list_t *);
DEFINE_FAKE_VALUE_FUNC2(int, iam__list_append, iam__list_t *, void *);
DEFINE_FAKE_VOID_FUNC2(iam__list_remove, iam__list_t *, void *);
DEFINE_FAKE_VOID_FUNC1(iam__list_clear, iam__list_t *);

void iam__list_clear_act(iam__list_t *list, action_fn act) {
    iam__list_clear(list);
}

void iam__list_free(iam__list_t *list) {
    iam__list_clear(list);
}

void iam__list_free_act(iam__list_t *list, action_fn act) {
    iam__list_clear(list);
}