#include "dns.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
 * Runs:
 *   dig <target>
 *
 * Example:
 *   dig example.com
 */
int run_dig_lookup(const char *target, const char *output_file) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd < 0) {
            perror("open failed");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 failed");
            close(fd);
            exit(EXIT_FAILURE);
        }

        close(fd);

        char *args[] = {
            "dig",
            (char *)target,
            NULL
        };

        execvp("dig", args);

        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    int status = 0;

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "dig lookup failed.\n");
        return -1;
    }

    return 0;
}

/*
 * Runs:
 *   dig -x <ip>
 *
 * Example:
 *   dig -x 192.168.1.1
 */
int run_reverse_dig_lookup(const char *ip, const char *output_file) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd < 0) {
            perror("open failed");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 failed");
            close(fd);
            exit(EXIT_FAILURE);
        }

        close(fd);

        char *args[] = {
            "dig",
            "-x",
            (char *)ip,
            NULL
        };

        execvp("dig", args);

        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    int status = 0;

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "reverse dig lookup failed.\n");
        return -1;
    }

    return 0;
}