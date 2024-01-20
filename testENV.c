#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char** getEnvironmentVariables() {
    // Command to print environment variables
    const char* command = "printenv";

    // Create a pipe to capture the output of the command
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for an array of strings to store environment variables
    char** envVariables = (char**)malloc(sizeof(char*));
    if (!envVariables) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    size_t index = 0;
    char buffer[256];

    // Read the output of the command and store each environment variable in the array
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t len = strlen(buffer);

        // Remove the newline character at the end
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        // Allocate memory for the environment variable and copy it
        envVariables[index] = (char*)malloc(len + 1);
        if (!envVariables[index]) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        strcpy(envVariables[index], buffer);
        index++;

        // Resize the array for the next environment variable
        envVariables = (char**)realloc(envVariables, (index + 1) * sizeof(char*));
        if (!envVariables) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }

    // Null-terminate the array
    envVariables[index] = NULL;

    // Close the pipe
    pclose(pipe);

    return envVariables;
}

int main() {
    char** envVariables = getEnvironmentVariables();

    // Print the environment variables
    for (size_t i = 0; envVariables[i] != NULL; ++i) {
        printf("%s\n", envVariables[i]);
        free(envVariables[i]);  // Free memory for each environment variable
    }

    free(envVariables);  // Free the array of environment variables

    return 0;
}

