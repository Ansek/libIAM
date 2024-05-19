// Copyright (c) 2024 Alexander Sekunov
// License: http://opensource.org/licenses/MIT

#include <iam/plugin.h>
#include <iam/algorithm.h>
#include <stdio.h>

static iam_metadata_t info = {
    .name = "NSA_RS",
    .version = "0.1",
    .description = "Negative Selection Algorithm based on Random Strings",
    .author = "Alexander Sekunov"
};

static void generate(iam_id_t id, const char *data, size_t size) {
    puts("NSA_RS.generate()");
}

static int analyze(iam_id_t id, const char *data, size_t size) {
    puts("NSA_RS.analyze()");
    return 0;
}

int nsa_rs_init(iam_id_t id) {
    iam_binary_alg_t *ba = iam_algorithm_reg_binary(id);
    iam_binary_alg_reg_generate(ba, generate);
    iam_binary_alg_reg_analyze(ba, analyze);
    return 0;
}

void nsa_rs_exit(iam_id_t id) {
}

IAM_PLUGIN_DYNAMIC_INIT(info, nsa_rs_init);
IAM_PLUGIN_DYNAMIC_EXIT(nsa_rs_exit);