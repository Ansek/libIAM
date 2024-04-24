// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <iam/iam.h>
#include <fff.h>

iam_metadata_t iam__api_md = {
    .name = "Test",
    .version = NULL,
    .description = NULL,
    .author = NULL
};
iam_id_t iam__api = { .info = &iam__api_md };

DEFINE_FFF_GLOBALS;