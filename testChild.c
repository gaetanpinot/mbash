#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    pid_t child_pid;

    child_pid = fork();

    if (child_pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) { // Child process
        // Replace the child process image with an infinite task (e.g., using execve)
	printf("%d\n",getpid());
        char* args[] = {"yes no", NULL};
        execvp("yes no", args);
        
        // If execvp fails, print an error
        perror("Execvp failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        printf("Parent process (PID %d) created child process with PID %d.\n", getpid(), child_pid);

        // Wait for a while (simulating other tasks in the parent)
        sleep(1);

        // Send a signal to the child process to terminate
        if (kill(child_pid, SIGTERM) == -1) {
            perror("Kill failed");
            exit(EXIT_FAILURE);
        }

        // Wait for the child process to complete
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child process with PID %d exited with status %d.\n", child_pid, WEXITSTATUS(status));
        } else {
            printf("Child process with PID %d did not exit normally.\n", child_pid);
        }
    }

    return 0;
}

