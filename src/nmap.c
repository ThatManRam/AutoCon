#include "nmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int run_nmap(const char *subnet) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {
        char *args[] = {
            "nmap",
            "-sS",
            "-p-",
            "-sV",
            "-O",
            "--version-all",
            (char *)subnet,
            "-oX",
            XML_FILE,
            NULL
        };

        execvp("nmap", args);

        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    int status = 0;

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Nmap did not exit cleanly.\n");
        return -1;
    }

    return 0;
}