// Copyright (c) 2024 Alexander Sekunov
// License: http://opensource.org/licenses/MIT

#include <iam/plugin.h>
#include <iam/algorithm.h>
#include <iam/setting.h>
#include <stdio.h>

static iam_metadata_t info = {
    .name = "NSA_RA",
    .version = "0.1",
    .description = "Negative Selection Algorithm based on Rectangular Areas",
    .author = "Alexander Sekunov"
};

static void generate(iam_id_t id, const float *vector, size_t k) {
    puts("NSA_RA.generate()");
}

static int analyze(iam_id_t id, const float *vector, size_t k) {
    puts("NSA_RA.analyze()");
    return 0;
}

int64_t t[3] = { 1, 2, 3 };
double coeff[3] = { 123.f, 1E10f, 3565.f };

int nsa_ra_init(iam_id_t id) {
    iam_setting_t *s;
    s = iam_setting_reg_int64_arr(id, "t", "Reset time", t, 3);
    s->info->count = 3;
    s = iam_setting_reg_double_arr(id, "coeff", "Coeficients", coeff, 3);
    s->info->count = 2;
    iam_real_alg_t *ra = iam_algorithm_reg_real(id);
    iam_real_alg_reg_generate(ra, generate);
    iam_real_alg_reg_analyze(ra, analyze);
    return 0;
}

void nsa_ra_exit(iam_id_t id) {
}

IAM_PLUGIN_DYNAMIC_INIT(info, nsa_ra_init);
IAM_PLUGIN_DYNAMIC_EXIT(nsa_ra_exit);