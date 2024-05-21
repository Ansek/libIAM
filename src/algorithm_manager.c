// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "algorithm_manager.h"
#include <string.h>

iam__list_t iam__binary_algs;
void iam__binary_algs_free(void *data);

iam__list_t iam__real_algs;
void iam__real_algs_free(void *data);

void iam__algorithm_manager_init(void) {
    iam__list_init(&iam__binary_algs);
    iam__list_init(&iam__real_algs);
}

void iam__algorithm_manager_exit(void) {
    iam__list_free_act(&iam__binary_algs, iam__binary_algs_free);
    iam__list_free_act(&iam__real_algs, iam__real_algs_free);
}

void iam_real_alg_fit(const char *alg_name,
    const double *inX, const uint8_t *inY, size_t row_n, size_t col_n) {
    iam__node_t *p;
    iam__real_alg_t *alg;
    IAM__FOREACH(p, iam__real_algs) {
        alg = IAM__D(real_alg, p);
        if (strcmp(alg_name, alg->id->info->name) == 0 
            && alg->real.fit != NULL)
            alg->real.fit(inX, inY, row_n, col_n);
    }
}

void iam_real_alg_predict(const char *alg_name,
    const double *inX, uint8_t *outY, size_t row_n, size_t col_n) {
    iam__node_t *p;
    iam__real_alg_t *alg;
    IAM__FOREACH(p, iam__real_algs) {
        alg = IAM__D(real_alg, p);
        if (strcmp(alg_name, alg->id->info->name) == 0 
            && alg->real.predict != NULL)
            alg->real.predict(inX, outY, row_n, col_n);
    }
}

iam_binary_alg_t *iam_algorithm_reg_binary(iam_id_t id) {
    int res;
    iam__binary_alg_t *alg = IAM__NEW(binary_alg);
    if (alg == NULL)
        return NULL;
    alg->id = (iam__module_t *)id;
    alg->binary.analyze = NULL;
    alg->binary.generate = NULL;
    iam__list_init(&alg->params);
    res = iam__list_append(&iam__binary_algs, alg);
    if (res == 1)
        return NULL;
	iam_logger_puts(id, IAM_TRACE,
		"Registered a binary algorithm");
    return (iam_binary_alg_t *)alg;
}

iam_real_alg_t *iam_algorithm_reg_real(iam_id_t id) {
    int res;
    iam__real_alg_t *alg = IAM__NEW(real_alg);
    if (alg == NULL)
        return NULL;
    alg->id = (iam__module_t *)id;
    alg->real.analyze = NULL;
    alg->real.generate = NULL;
    iam__list_init(&alg->params);
    res = iam__list_append(&iam__real_algs, alg);
    if (res == 1)
        return NULL;
	iam_logger_puts(id, IAM_TRACE,
		"Registered a real algorithm");
    return (iam_real_alg_t *)alg;
}

void iam_binary_alg_reg_generate(iam_binary_alg_t *alg,
    iam_binary_generate_fn fn) {
    alg->generate = fn;
	iam_logger_puts(IAM__ID(binary_alg, alg), IAM_TRACE,
		"Added generation function (binary)");  
}

void iam_binary_alg_reg_analyze(iam_binary_alg_t *alg,
    iam_binary_analyze_fn fn) {
    alg->analyze = fn;
	iam_logger_puts(IAM__ID(binary_alg, alg), IAM_TRACE,
		"Added analysis function (binary)"); 
}

void iam_real_alg_reg_generate(iam_real_alg_t *alg,
    iam_real_generate_fn fn) {
    alg->generate = fn;
	iam_logger_puts(IAM__ID(real_alg, alg), IAM_TRACE,
		"Added generation function (real)"); 
}

IAM_API void iam_real_alg_reg_analyze(iam_real_alg_t *alg,
    iam_real_analyze_fn fn) {
    alg->analyze = fn;
	iam_logger_puts(IAM__ID(real_alg, alg), IAM_TRACE,
		"Added analysis function (real)"); 
}

void iam_real_alg_reg_fit(iam_real_alg_t *alg,
    iam_real_fit_fn fn) {
    alg->fit = fn;
	iam_logger_puts(IAM__ID(real_alg, alg), IAM_TRACE,
		"Added fit function (real)"); 
}

void iam_real_alg_reg_predict(iam_real_alg_t *alg,
    iam_real_predict_fn fn) {
    alg->predict = fn;
	iam_logger_puts(IAM__ID(real_alg, alg), IAM_TRACE,
		"Added predict function (real)");
}

void iam__binary_algs_free(void *data) {
    // Проверка
    ((iam_binary_alg_t *)data)->generate(NULL, NULL, 0);
    ((iam_binary_alg_t *)data)->analyze(NULL, NULL, 0); 
}

void iam__real_algs_free(void *data) {
    // Проверка
    ((iam_real_alg_t *)data)->generate(NULL, NULL, 0);
    ((iam_real_alg_t *)data)->analyze(NULL, NULL, 0);
}