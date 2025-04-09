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
#define RAND(min, max) (double) rand() / RAND_MAX * (max - min) + min
#define RAND_DET RAND(-4, 4)
#define RAND_R RAND(0, 3)

uint8_t det_id;
bool isVdetectors = true;
double **detectors_array[8];
double *r_array[8];
bool *detectors_is_valid[8];
uint64_t *activations_count_f[8];
uint64_t *activations_count_p[8];
uint64_t attr_n = 46;
uint64_t det_n = DET_N;
double det_r = 1.6;
char msg[255], *msg_p;

void fit(const double *inX, const uint8_t *inY, size_t row_n, size_t col_n) {
    uint8_t attempt = 0, attempt_max = 100;
    size_t k, i, j;
    double euclidean, radius, r_min, sum, step, s1, s2; 
    const double *begX = inX, *r_minX;
    double **detectors = detectors_array[det_id];
    bool *is_valid = detectors_is_valid[det_id];
    double *sq_diff = (double *)malloc(sizeof(double) * col_n);
    for (k = 0; k < det_n; k++) {
        inX = begX;
        r_min = 1E308;
        r_minX = NULL;
        radius = isVdetectors ? r_array[det_id][k] : det_r;
        for (i = 0; i < row_n; i++) {
            if (attempt == attempt_max)
                break;
            euclidean = 0;
            for (j = 0; j < col_n; j++)
                euclidean += pow(detectors[k][j] - inX[j], 2);
            euclidean = sqrt(euclidean);
            if (inY[i] == 0) {
                if (euclidean <= radius) {
                    for (j = 0; j < col_n; j++)
                        detectors[k][j] = RAND_DET;
                    if (isVdetectors)
                        r_array[det_id][k] = RAND_R;
                    activations_count_f[det_id][k] = 0;
                    activations_count_p[det_id][k] = 0;
                    i--; // Повторно вектор c новым детектором
                    attempt++;
                    continue;
                } else if (attempt > 0) {
                    i = 0; // Новый детектор повторно со всеми векторами (сначала)
                    inX = begX;
                    attempt = 0;
                    continue;
                }
            } else {
                if (euclidean < r_min) {
                    r_min = euclidean;
                    r_minX = inX;
                }
                if (euclidean <= radius)
                    activations_count_f[det_id][k]++;
            }
            inX += col_n;
        }
        if (activations_count_f[det_id][k] == 0 &&
            activations_count_p[det_id][k] < 3 && r_minX != NULL) {
            for (j = 1; j < col_n; j++)
                sq_diff[j] = pow(detectors[k][j] - r_minX[j], 2);
            j = 0;
            i = (int)(r_minX - begX) / (sizeof(double));
            while (r_min > radius && attempt < attempt_max) {
                step = (detectors[k][j] - r_minX[j]) / 4;
                sum = 0;
                for (i = 0; i < col_n; i++)
                    if (i != j)
                        sum += sq_diff[i];
                while (euclidean >= r_min && attempt < attempt_max) {
                    s2 = detectors[k][j] - r_minX[j];
                    s1 = fabs(s2);
                    s2 = fabs(s2 - step);
                    if (s2 < s1) {
                        detectors[k][j] -= step;
                        s2 *= s2;
                        euclidean = sqrt(s2 + sum);
                        attempt = 0;
                    } else {
                        if (s2 > s1) 
                            step = -step;
                        else
                            step = step / 2;
                        attempt++;
                    }
                }
                r_min = euclidean;
                sq_diff[j] = s2;
                j++;
                if (j == col_n)
                    j = 0;
            }
            if (attempt < attempt_max) {
                activations_count_p[det_id][k] = 0;
                k--; // Повторная проверка
                attempt = 0;
            }
        }
        if (attempt == attempt_max) {
            msg_p += sprintf(msg_p, " %Id", k);
            attempt = 0;
            is_valid[k] = false;
        } else {
            is_valid[k] = true;
        }
    }
    free(sq_diff);
}

void predict(const double *inX, uint8_t *outY, size_t row_n, size_t col_n) {
    size_t k, i, j;
    double euclidean, radius;
    double **detectors = detectors_array[det_id];
    bool *is_valid = detectors_is_valid[det_id];
    for (i = 0; i < row_n; i++) {
        outY[i] = 0;
        for (k = 0; k < det_n; k++) {
            if (!is_valid[k])
                continue;
            euclidean = 0;
            radius = isVdetectors ? r_array[det_id][k] : det_r;
            for (j = 0; j < col_n; j++)
                euclidean += pow(detectors[k][j] - inX[j], 2);
            euclidean = sqrt(euclidean);
            if (euclidean < radius) {
                activations_count_p[det_id][k]++;
                outY[i] = 1;
                break;
            }
        }
        inX += col_n;
    }
}

#define NEW_M(type, var, count) \
    var = (type *)malloc(sizeof(type) * count)
#define NEW_C(type, var, count) \
    var = (type *)calloc(sizeof(type) * count, sizeof(type))

static void load_setting(iam_id_t id) {
    size_t q, j, k;
    for (q = 0; q < 8; q++) {
        NEW_C(uint64_t, activations_count_f[q], det_n);
        NEW_C(uint64_t, activations_count_p[q], det_n);
        NEW_M(double, r_array[q], det_n);  
        NEW_M(double *, detectors_array[q], det_n);
        NEW_C(bool, detectors_is_valid[q], det_n); 
        for (k = 0; k < det_n; k++) {
            NEW_M(double, detectors_array[q][k], attr_n);
            for (j = 0; j < attr_n; j++) {
                detectors_array[q][k][j] = RAND_DET;
            }
            r_array[q][k] = RAND_R;
        }         
    }
    msg_p = msg;
    msg_p += sprintf(msg_p, "%s", "Вad detectors:");
}

int nsa_rv_init(iam_id_t id) {
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

void nsa_rv_exit(iam_id_t id) {
    size_t i, j;
    FILE *f = fopen("activations_count.txt", "w");
    if (f == NULL) {
        printf("Failed to open the file activations_count.txt.");
        return;
    }
    for (i = 0; i < 8; i++){
        for (j = 0; j < det_n; j++) {
            free(detectors_array[i][j]);
            fprintf(f, "%"PRId64",%"PRId64,
                activations_count_f[i][j],
                activations_count_p[i][j]);
            if (j < det_n - 1)
                fputs(",", f);
        }
        fputs("\n", f);
        free(r_array[i]);
        free(detectors_array[i]);
        free(detectors_is_valid[i]);
        free(activations_count_f[i]);
        free(activations_count_p[i]);
    }
    fputs(msg, f);
    fclose(f);
}

IAM_PLUGIN_DYNAMIC_INIT(info, nsa_rv_init);
IAM_PLUGIN_DYNAMIC_EXIT(nsa_rv_exit);