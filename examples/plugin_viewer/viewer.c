// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <iam/init.h>
#include <iam/info.h>
#include <iam/setting.h>
#include <inttypes.h>
#include <stdio.h>

void print_info(void) {
    size_t i, count;
    iam_id_t id;
    iam_setting_t *s;
    iam_module_rewind();
    while(id = iam_module_read()) {
        printf("Module: %s, V. %s\nDescription: %s\nAuthor: %s\nSetting:\n",
            id->info->name, id->info->version,
            id->info->description, id->info->author);
        iam_setting_rewind(id);
        count = 0;
        while (s = iam_setting_read(id)) {
            printf("- %s: ", s->info->name);
            if (s->info->max != 1)
                printf("[");
            for (i = 0; i < s->info->count; i++) {
                printf(iam_setting_to_str_i(s, i));
                if (i < s->info->count - 1 && s->info->max != 1)
                    printf(", ");
            }
            if (s->info->max != 1)
                printf("] (%s).\n", s->info->desc);
            else
                printf(" (%s).\n", s->info->desc);            
            count++;
        }
        if (count > 0)
            printf("Total settings = %"PRId64"\n\n", count);
        else
            puts("Absent.\n");
    }
}

int main() {
    iam_init();
    print_info();
    iam_exit();
    puts("end");
    return 0;
}