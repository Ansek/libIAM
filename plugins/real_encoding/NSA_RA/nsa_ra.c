// Copyright (c) 2024 Alexander Sekunov
// License: http://opensource.org/licenses/MIT

#include <iam/plugin.h>
#include <stdio.h>

iam_id_t info = {
    .name = "NSA_RA",
    .version = "0.1",
    .description = "Negative Selection Algorithm based on Rectangular Areas",
    .author = "Alexander Sekunov"
};

iam_id_t *iam_nsa_ra_init(void) {
    puts("iam_nsa_ra_init");
    return &info;
}

void iam_nsa_ra_exit(void) {
    puts("iam_nsa_ra_exit");
}

IAM_PLUGIN_DYNAMIC_INIT(iam_nsa_ra_init);
IAM_PLUGIN_DYNAMIC_EXIT(iam_nsa_ra_exit);