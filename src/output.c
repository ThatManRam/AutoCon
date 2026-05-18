#include "output.h"

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

int create_output_directory(void) {
    if (mkdir(OUTPUT_DIR, 0755) == -1) {
        if (errno != EEXIST) {
            perror("mkdir output failed");
            return -1;
        }
    }

    return 0;
}