// Copyright (c) 2024 Alexander Sekunov
// License: http://opensource.org/licenses/MIT

#include <iam/plugin.h>
#include <iam/algorithm.h>
#include <iam/setting.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static iam_metadata_t info = {
    .name = "NSA_RV",
    .version = "0.1",
    .description = "Negative Selection Algorithm based on Real-Valued",
    .author = "Alexander Sekunov"
};

#define DET_N 200

uint8_t det_id;
bool isVdetectors = true;
double **detectors_array[8];
uint64_t attr_n = 46;
uint64_t det_n = DET_N;
double det_r = 0.5;
double d[46] = { 3.94E05, 9.91E06, 47.0, 255.0, 8.39E6, 8.39E6, 29.72, 1.0, 1.0, 
    1.0, 1.0, 1.0, 1.0, 1.0, 7.7, 12.87, 248.32, 4401.7, 9613.0, 1.0, 1.0, 1.0, 
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.27E5, 13583.0, 
    49014.0, 13583.0, 12385.24, 13583.0, 1.68E8, 15.00, 164.82, 17551.27,
    1.55E+08, 1.00, 244.60 };
double r[DET_N];

void fit(const double *inX, const uint8_t *inY, size_t row_n, size_t col_n) {
    uint8_t attempt = 0, attempt_max = 10;
    size_t k, i, j;
    double euclidean, radius;
    const double *begX = inX;
    double **detectors = detectors_array[det_id];
    for (k = 0; k < det_n; k++) {
        inX = begX;
        radius = isVdetectors ? r[k] : det_r;
        for (i = 0; i < row_n; i++) {
            if (inY[i] == 0) {
                euclidean = 0;
                for (j = 0; j < col_n; j++)
                    euclidean += pow(detectors[k][j] - inX[j], 2);
                euclidean = sqrt(euclidean);
                if (euclidean < radius && attempt < attempt_max) {
                    for (j = 0; j < attr_n; j++)
                        detectors[k][j] = rand() / d[j];
                    if (isVdetectors)
                        r[k] = rand() / (2 * d[j]);
                    i--; // Повторно вектор c новым детектором
                    attempt++;
                    continue;
                }
            }
            if (attempt > 0 && attempt < attempt_max) {
                k--; // Новый детектор повторно со всеми векторами (сначала)
                break;
            }
            inX += col_n;
        }
        attempt = 0;
    }
}

void predict(const double *inX, uint8_t *outY, size_t row_n, size_t col_n) {
    size_t k, i, j;
    double euclidean, radius;
    double **detectors = detectors_array[det_id];
    for (i = 0; i < row_n; i++) {
        outY[i] = 0;
        for (k = 0; k < det_n; k++) {
            euclidean = 0;
            radius = isVdetectors ? r[k] : det_r;
            for (j = 0; j < col_n; j++)
                euclidean += pow(detectors[k][j] - inX[j], 2);
            euclidean = sqrt(euclidean);
            if (euclidean < radius) {
                outY[i] = 1;
                break;
            }
        }
        inX += col_n;
    }
    outY[0] = 1;
}

double **create_detectors(void) {
    size_t k, j;
    double **detectors = (double **)malloc(sizeof(double*) * det_n);
    if (detectors != NULL) {
        for (k = 0; k < det_n; k++) {
            detectors[k] = (double *)malloc(sizeof(double) * det_n);
            if (detectors[k] != NULL) {
                for (j = 0; j < attr_n; j++)
                    detectors[k][j] = rand() / d[j];
                if (isVdetectors)
                    r[k] = rand() / (2 * d[j]);
            }
        }
    }
    return detectors;
}

static void load_setting(iam_id_t id) {
    size_t j;
    for (j = 0; j < attr_n; j++)
        d[j] = RAND_MAX / d[j];
    for (j = 0; j < 8; j++)
        detectors_array[j] = create_detectors();
}

int nsa_ra_init(iam_id_t id) {
    iam_setting_reg_uint64(id, "attr_n", "Number of attributes.", &attr_n);
    iam_setting_reg_uint64(id, "det_n", "Number of detectors.", &det_n);
    iam_setting_reg_udouble(id, "det_r", "Detector radius.", &det_r);
    iam_setting_reg_bool(id, "isVdetectors",
        "Is variable size detector.", &isVdetectors);
    iam_setting_reg_uint8(id, "det_id", "Detector set ID", &det_id);
    iam_setting_reg_callback(id, load_setting);
    iam_real_alg_t *ra = iam_algorithm_reg_real(id);
    iam_real_alg_reg_fit(ra, fit);
    iam_real_alg_reg_predict(ra, predict);
    return 0;
}

void nsa_ra_exit(iam_id_t id) {
    size_t i, j;
    for (i = 0; i < 8; i++){
        for (j = 0; j < det_n; j++)  
            free(detectors_array[i][j]);
        free(detectors_array[i]);
    }
}

IAM_PLUGIN_DYNAMIC_INIT(info, nsa_ra_init);
IAM_PLUGIN_DYNAMIC_EXIT(nsa_ra_exit);