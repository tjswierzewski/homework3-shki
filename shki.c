#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define BUFFER_SIZE 100
#define COMMAND_SIZE 25
#define PATH_MAX 100

int child_pid;

void handler(int sig)
{
    if (child_pid != 0)
    {
        kill(child_pid, sig);
    }
}

void green()
{
    printf("\033[1;32m");
}

void reset()
{
    printf("\033[0m");
}

int parse_command_input(char *buffer, char *command_array[])
{
    if (strlen(buffer) == 0)
    {
        return 0;
    }

    char *endline = strstr(buffer, "\n");
    *endline = '\0';

    int i, j = 1;
    command_array[0] = buffer;
    for (i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == ' ')
        {
            buffer[i] = '\0';
            command_array[j] = buffer + (i + 1);
            j++;
        }
    }
    command_array[j] = NULL;
    return j;
}

int main(int argc, char const *argv[])
{
    child_pid = 0;
    char input_buffer[BUFFER_SIZE];
    char *commands_head[COMMAND_SIZE];
    char **commands = commands_head;
    char **next_command = NULL;
    int child_out = 1, child_in = 0;
    int pipe_fd[2] = {-1, -1};
    while (1)
    {
        signal(2, &handler);
        int commands_length;
        char cwd[PATH_MAX];
        int status, background = 0;
        char augment;

        if (next_command == NULL)
        {
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
            commands = commands_head;
            commands_length = parse_command_input(input_buffer, commands);
        }
        else
        {
            commands = next_command;
        }

        if (commands[0] == NULL)
        {
            printf("\n");
            continue;
        }

        if (strcmp(commands[0], "exit") == 0)
        {
            exit(0);
        }

        if (strcmp(commands[commands_length - 1], "&") == 0)
        {
            background = 1;
        }

        int i;
        for (i = 0; i < commands_length; i++)
        {
            if (strcmp(commands[i], ">") == 0)
            {
                commands[i] = NULL;
                child_out = open(commands[++i], O_CREAT | O_TRUNC | O_WRONLY, 00600);
                if (child_out < 0)
                {
                    printf("Error opening file: %s", commands[i]);
                }
                break;
            }
            if (strcmp(commands[i], "|") == 0)
            {
                commands[i] = NULL;
                commands_length -= i + 1;
                next_command = &commands[++i];
                pipe(pipe_fd);
                child_out = pipe_fd[1];
                break;
            }
        }

        if (child_pid = fork() == 0)
        {
            if (child_out != 1)
            {
                dup2(child_out, 1);
            }
            if (child_in != 0)
            {
                dup2(child_in, 0);
            }

            if (background == 1)
            {
                commands[commands_length - 1] = NULL;
            }

            execvp(commands[0], commands);
        }
        else
        {
            if (pipe_fd[0] > 0 && pipe_fd[1] > 0)
            {
                child_in = pipe_fd[0];
                child_out = 1;
                close(pipe_fd[1]);
                pipe_fd[0] = -1;
                pipe_fd[1] = -1;
                continue;
            }
            else
            {
                next_command = NULL;
            }

            if (child_out != 1)
            {
                close(child_out);
                child_out = 1;
            }

            if (background == 0)
            {
                waitpid(child_pid, &status, 0);
            }
        }
        background = 0;
    }
    return 0;
}
