// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <iam/init.h>
#include <stdio.h>

int main() {
    iam_init();
    iam_exit();
    puts("end");
    return 0;
}