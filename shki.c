#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100
#define COMMAND_SIZE 25
#define PATH_MAX 100

void red()
{
    printf("\033[1;31m");
}

void green()
{
    printf("\033[1;32m");
}

void reset()
{
    printf("\033[0m");
}

void parse_command_input(char *buffer, char *command_array[])
{
    if (strlen(buffer) == 0)
    {
        return;
    }
    char *endline = strstr(buffer, "\n");
    *endline = '\0';
    char *command;
    int i = 0;

    command = strtok(buffer, " ");
    for (; command != NULL; i++)
    {
        command_array[i] = malloc(strlen(command));
        strcpy(command_array[i], command);
        command = strtok(NULL, " ");
    }
    command_array[i] = NULL;
}

int main(int argc, char const *argv[])
{
    while (1)
    {
        char cwd[PATH_MAX];
        char input_buffer[BUFFER_SIZE];
        char *commands[COMMAND_SIZE];
        int child_pid, status;
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd: ");
            exit(1);
        }
        green();
        printf("%s > ", cwd);
        reset();
        if (fgets(input_buffer, BUFFER_SIZE, stdin) == NULL && feof(stdin))
        {
            printf("Could not read STDIN. Exiting.");
            exit(1);
        }
        parse_command_input(input_buffer, commands);
        if (child_pid = fork() == 0)
        {
            execvp(commands[0], commands);
        }
        else
        {
            waitpid(child_pid, &status, 0);
        }
    }
    return 0;
}
