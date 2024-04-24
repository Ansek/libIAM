// Copyright (c) 2024 Alexander Sekunov
// License: http://opensource.org/licenses/MIT

#include <iam/plugin.h>
#include <iam/algorithm.h>
#include <stdio.h>

static iam_metadata_t info = {
    .name = "NSA_RA",
    .version = "0.1",
    .description = "Negative Selection Algorithm based on Rectangular Areas",
    .author = "Alexander Sekunov"
};

static void generate(iam_id_t *handle, const float* vector, size_t k) {
    puts("NSA_RA.generate()");
}

static int analyze(iam_id_t *handle, const float* vector, size_t k) {
    puts("NSA_RA.analyze()");
    return 0;
}

int nsa_ra_init(void) {
    iam_id_t *id = iam_plugin_register(&info);
    iam_real_alg_t *ra = iam_algorithm_reg_real(id);
    iam_real_alg_reg_generate(ra, generate);
    iam_real_alg_reg_analyze(ra, analyze);
    return 0;
}

void nsa_ra_exit(void) {
}

IAM_PLUGIN_DYNAMIC_INIT(nsa_ra_init);
IAM_PLUGIN_DYNAMIC_EXIT(nsa_ra_exit);