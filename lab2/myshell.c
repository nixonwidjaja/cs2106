/**
 * CS2106 AY22/23 Semester 2 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "myshell.h"

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int PCBIndex, PCBId;
struct PCBTable *PCBArray;

static void proc_update_status(int index, int status) {
    if (WIFEXITED(status)) {
        PCBArray[index].status = 1;
        PCBArray[index].exitCode = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        PCBArray[index].status = 1;
        PCBArray[index].exitCode = WTERMSIG(status);
    }
} 


static void proc_update_all() {
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    while (pid > 0) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (PCBArray[i].pid == pid) {
                proc_update_status(i, status);
                break;
            }
        }
        pid = waitpid(-1, &status, WNOHANG);
    }
}

/***************************
 * Built-in Commands
 **************************/

static void command_info(char *opt) {
    proc_update_all();

    if (strcmp(opt, "0") == 0) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (PCBArray[i].pid != -1) {
                printf("[%i] ", PCBArray[i].pid);
                if (PCBArray[i].status == 1) {
                    printf("Exited %i\n", PCBArray[i].exitCode);
                } else if (PCBArray[i].status == 2) {
                    printf("Running\n");
                } else if (PCBArray[i].status == 3) {
                    printf("Terminating\n");
                }
            }
        }
    } else if (strcmp(opt, "1") == 0) {
        int count = 0;
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (PCBArray[i].pid != -1) {
                if (PCBArray[i].status == 1) {
                    count++;
                }
            }
        }
        printf("Total exited process: %i\n", count);
    } else if (strcmp(opt, "2") == 0) {
        int count = 0;
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (PCBArray[i].pid != -1) {
                if (PCBArray[i].status == 2) {
                    count++;
                }
            }
        }
        printf("Total running process: %i\n", count);
    } else if (strcmp(opt, "3") == 0) {
        int count = 0;
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (PCBArray[i].pid != -1) {
                if (PCBArray[i].status == 3) {
                    count++;
                }
            }
        }
        printf("Total terminating process: %i\n", count);
    } else {
        fprintf(stderr, "Wrong command\n");
    }
}

static void command_wait(int pid) {
    proc_update_all();

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (PCBArray[i].pid == pid) {
            if (PCBArray[i].status == 2) {
                int status;
                waitpid(PCBArray[i].pid, &status, 0);
                proc_update_status(i, status);
            }
            break;
        }
    }
}

static void command_terminate(int pid) {

    proc_update_all();

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (PCBArray[i].pid == pid) {
            if (PCBArray[i].status == 2) {
                PCBArray[i].status = 3;
                kill(PCBArray[i].pid, SIGTERM);
            }
            break;
        }
    }

}


/***************************
 * Program Execution
 **************************/

static void command_exec(size_t num_tokens, char **tokens, int PCBIndex, char *input, char *output, char *error_output) {
    char *path = tokens[0];
    if (access(path, F_OK) != -1) {
        char *program_name = basename(path);
        if (tokens[num_tokens - 2][0] == '&') {
            char *args[num_tokens - 1];
            args[0] = path;
            for (int i = 1; i < (num_tokens - 2); i++) {
                args[i] = tokens[i];
            }
            args[num_tokens - 2] = NULL;
            process_background(path, args, PCBIndex, input, output, error_output);
        } else {
            char *args[num_tokens];
            args[0] = path;
            for (int i = 1; i < num_tokens; i++) {
                args[i] = tokens[i];
            }
            process_now(path, args, PCBIndex, input, output, error_output);
        }
    } else {
        fprintf(stderr, "%s not found\n", path);
    }
}

/***************************
 * Command Processor
 **************************/

static void command(size_t num_tokens, char **tokens, int PCBIndex, char *input, char *output, char *error_output) {
    if (strcmp(tokens[0], "info") == 0) {
        char *opt = num_tokens > 2 ? tokens[1] : "err";
        command_info(opt);
    } else if (strcmp(tokens[0], "wait") == 0) {
        command_wait(atoi(tokens[1]));
    } else if (strcmp(tokens[0], "terminate") == 0) {
        command_terminate(atoi(tokens[1]));
    } else {
        command_exec(num_tokens, tokens, PCBIndex, input, output, error_output);
    }
}

static void process_redirection(size_t num_tokens, char **tokens, int PCBIndex) {
    char *input = "", *output = "", *error_output = "";
    int index = -1;

    for (int i = 0; i < num_tokens - 2; i++) {
        if (strcmp(tokens[i], "<") == 0) {
            input = tokens[i + 1];
            index = i;
        } 
        
        if (strcmp(tokens[i], ">") == 0) {
            output = tokens[i + 1];
            if (index == -1) {
                index = i;
            }
        }
         
        if (strcmp(tokens[i], "2>") == 0) {
            error_output = tokens[i + 1];
            if (index == -1) {
                index = i;
            }
        }
    }

    if (index == -1) {
        command(num_tokens, tokens, PCBIndex, input, output, error_output);
        return;
    }

    int size = index + 1;
    if (tokens[num_tokens - 2][0] == '&') {
        size++;
    }

    char **out[size];
    for (int i = 0; i < size; i++) {
        out[i] = tokens[i];
    }

    if (tokens[num_tokens - 2][0] == '&') {
        out[size - 2] = "&";
    }

    out[size - 1] = NULL;
    command(size, out, PCBIndex, input, output, error_output);
}

/***************************
 * High-level Procedure
 **************************/

void my_init(void) {
    PCBIndex = 0;
    PCBId = shmget(IPC_PRIVATE, sizeof(struct PCBTable) * MAX_PROCESSES, IPC_CREAT | 0600);
    PCBArray = (struct PCBTable *)shmat(PCBId, NULL, 0);
    for (int i = 0; i < MAX_PROCESSES; i++) {
        PCBArray[i].pid = -1;
    }
}

void my_process_command(size_t num_tokens, char **tokens) {
    if (num_tokens > 1) {
        int start = 0;
        while (start < num_tokens) {
            int end = start;
            while (end < num_tokens - 1) {
                if (strcmp(tokens[end], ";") == 0) {
                    break;
                }
                end++;
            }
            int size = end - start + 1;
            char *out[size];
            for (int i = start; i < end; i++) {
                out[i - start] = tokens[i];
            }
            out[size - 1] = NULL;
            process_redirection(size, out, PCBIndex);
            PCBIndex++;
            start = end + 1;
        }
    }
}

void process_now(char *path, char **args, int PCBIndex, char *input, char *output, char *error_output) {
    pid_t pid = fork();

    if (pid == 0) {
        PCBArray[PCBIndex].pid = getpid();
        PCBArray[PCBIndex].status = 2;
        PCBArray[PCBIndex].exitCode = -1;
        if (strcmp(input, "") != 0) {
            FILE *in = fopen(input, "r");
            if (in == NULL) {
                fprintf(stderr, "%s does not exist\n", input);
                exit(EXIT_FAILURE);
                return;
            }
            dup2(fileno(in), STDIN_FILENO);
            fclose(in);
        }
        if (strcmp(output, "") != 0) {
            FILE *out = fopen(output, "w+");
            if (out == NULL) {
                perror("Failed to open");
                exit(EXIT_FAILURE);
            }
            dup2(fileno(out), STDOUT_FILENO);
            fclose(out);
        }
        if (strcmp(error_output, "") != 0) {
            FILE *error_file = fopen(error_output, "w");
            if (error_file == NULL) {
                perror("Failed to open");
                exit(EXIT_FAILURE);
            }
            if (dup2(fileno(error_file), STDERR_FILENO) == -1) {
                perror("Failed to copy");
                exit(EXIT_FAILURE);
            }
            fclose(error_file);
        }

        execv(path, args);
        perror("Failed to execute");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        proc_update_status(PCBIndex, status);
    }
}

void process_background(char *path, char **args, int PCBIndex, char *input, char *output, char *error_output) {
    pid_t pid = fork();
    if (pid == 0) {
        PCBArray[PCBIndex].pid = getpid();
        PCBArray[PCBIndex].status = 2;
        PCBArray[PCBIndex].exitCode = -1;
        if (strcmp(input, "") != 0) {
            FILE *in = fopen(input, "r");
            if (in == NULL) {
                fprintf(stderr, "%s does not exist\n", input);
                exit(EXIT_FAILURE);
                return;
            }
            dup2(fileno(in), STDIN_FILENO);
            fclose(in);
        }
        if (strcmp(output, "") != 0) {
            FILE *out = fopen(output, "w+");
            if (out == NULL) {
                perror("Failed to open");
                exit(EXIT_FAILURE);
            }
            dup2(fileno(out), STDOUT_FILENO);
            fclose(out);
        }
        if (strcmp(error_output, "") != 0) {
            FILE *error_file = fopen(error_output, "w");
            if (error_file == NULL) {
                perror("Failed to open");
                exit(EXIT_FAILURE);
            }
            if (dup2(fileno(error_file), STDERR_FILENO) == -1) {
                perror("Failed to copy");
                exit(EXIT_FAILURE);
            }
            fclose(error_file);
        }

        execv(path, args);
        perror("Failed to execute");
        exit(EXIT_FAILURE);
    } else {
        printf("Child [%i] in background\n", pid);
    }
}

void my_quit(void) {
    proc_update_all();

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (PCBArray[i].pid != -1 && PCBArray[i].status == 2) {
            kill(PCBArray[i].pid, SIGTERM);
            printf("Killing [%i]\n", PCBArray[i].pid);
        }
    }
    printf("\nGoodbye\n");
    shmdt((char *)PCBArray);
    shmctl(PCBId, IPC_RMID, 0);
}
