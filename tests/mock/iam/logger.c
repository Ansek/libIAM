// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include "logger.h"

DEFINE_FAKE_VOID_FUNC2(iam_logger_put, iam_id_t, iam_logger_level);

void iam_logger_puts(iam_id_t id, iam_logger_level level,
    const char *msg) {
    iam_logger_put(id, level);
}
     
void iam_logger_putf(iam_id_t id, iam_logger_level level,
    const char *msg, ...) {
    iam_logger_put(id, level);
}