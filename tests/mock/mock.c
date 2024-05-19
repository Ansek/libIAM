// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <iam/iam.h>
#include <common.h>
#include <fff.h>

iam_metadata_t iam__api_md = {
    .name = "Test",
    .version = NULL,
    .description = NULL,
    .author = NULL
};
iam__module_t iam_api_m = { .info = &iam__api_md };
iam_id_t iam__api = (iam_id_t)&iam_api_m;

DEFINE_FFF_GLOBALS;