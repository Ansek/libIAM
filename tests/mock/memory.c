// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "memory.h"

DEFINE_FAKE_VALUE_FUNC1(void *, iam__malloc, size_t);
DEFINE_FAKE_VOID_FUNC1(iam__free, void *);