#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(void) {
    struct tm *info;
    time_t raw_time = time(NULL);
    info = localtime(&raw_time);

    char command[512];
    char time_string[100];

    // Store the timestamp in time_string
    if (strftime(
            time_string,
            sizeof(time_string),
            "%Y-%m-%d-%H-%M-%S",
            info
        ) == 0) {
        fprintf(stderr, "Failed to format timestamp.\n");
        return EXIT_FAILURE;
    }

snprintf(
    command,
    sizeof(command),
    "python3 MEGA_Upload.py \"%s\" \"$(pwd)/output\"",
    time_string
);

system(command);

    return EXIT_SUCCESS;
}