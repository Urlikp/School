#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SYS_ERROR 2

void signal_handler(int sig) {
    if (sig == SIGTERM) {
        const char *message = "GEN TERMINATED\n";
        size_t len = strlen(message) + 1;
        write(STDERR_FILENO, message, len);
        _exit(EXIT_SUCCESS);
    }
}

void check_error(int returned_value) {
    if (returned_value < 0) {
        _exit(SYS_ERROR);
    }
}

int main(int argc, char *argv[]) {
    int pfd[2];
    pid_t nsd, gen;

    check_error(pipe(pfd));

    gen = fork();
    if (gen == 0) {
        if (signal(SIGTERM, signal_handler) == SIG_ERR) {
            _exit(SYS_ERROR);
        }

        check_error(close(STDOUT_FILENO));
        check_error(close(pfd[STDIN_FILENO]));
        check_error(dup2(pfd[STDOUT_FILENO], STDOUT_FILENO));
        check_error(close(pfd[STDOUT_FILENO]));

        while (1) {
            check_error(printf("%d %d\n", rand() % 4096, rand() % 4096));
            fflush(stdout);
            check_error(sleep(1));
        }

        _exit(EXIT_SUCCESS);
    } else if (gen == -1) {
        exit(SYS_ERROR);
    }

    if (gen) {
        nsd = fork();
        if(nsd == 0) {
            check_error(close(STDIN_FILENO));
            check_error(close(pfd[STDOUT_FILENO]));
            check_error(dup2(pfd[STDIN_FILENO], STDIN_FILENO));
            check_error(close(pfd[STDIN_FILENO]));
            check_error(execl("nsd", "nsd", NULL));

            _exit(EXIT_SUCCESS);
        } else if (nsd == -1) {
            exit(SYS_ERROR);
        }
    }

    if (gen && nsd) {
        check_error(close(pfd[STDIN_FILENO]));
        check_error(close(pfd[STDOUT_FILENO]));
        check_error(sleep(5));
        check_error(kill(gen, SIGTERM));

        int child_status;
        int gen_error = 0, nsd_error = 0;

        if (waitpid(gen, &child_status, 0) < 0) {
            gen_error = 1;
        }

        if (WIFEXITED(child_status)) {
            gen_error = WEXITSTATUS(child_status);
        } else {
            gen_error = 1;
        }

        if (waitpid(nsd, &child_status, 0) < 0) {
            nsd_error = 1;
        }

        if (WIFEXITED(child_status)) {
            nsd_error = WEXITSTATUS(child_status);
        } else {
            nsd_error = 1;
        }

        if (gen_error || nsd_error) {
            printf("ERROR\n");
            exit(EXIT_FAILURE);
        } else {
            printf("OK\n");
        }
    }
    
    return 0;
}

