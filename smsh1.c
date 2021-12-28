/**  smsh1.c  small-shell version 1
 **             first really useful version after prompting shell
 **             this one parses the command line into strings
 **             uses fork, exec, wait, and ignores signals
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>
#include "smsh.h"

int bgd_processes;
// int run_in_background();

int main()
{
    char *cmdline, *prompt, *curr_dir, **arglist;
    int result;
    void setup();
    bgd_processes = 1;
    prompt = (char *)malloc(BUFSIZ);
    curr_dir = (char *)malloc(BUFSIZ);
    get_prompt(prompt, curr_dir);

    setup();

    while ((cmdline = next_cmd(prompt, stdin)) != NULL)
    {
        if ((arglist = splitline(cmdline)) != NULL)
        {
            // check for builtin commands first
            if (strcmp(arglist[0], "exit") == 0)
            {
                exit_shell(arglist);
            }
            else if (strcmp(arglist[0], "cd") == 0)
            {
                cd(arglist);
            }
            // or call exec
            else
            {
                result = execute(arglist);
            }

            // refresh the prompt
            get_prompt(prompt, curr_dir);
            freelist(arglist);
        }
    }

    free(prompt);
    free(curr_dir);
    return 0;
}

void setup()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

char *get_prompt(char *prompt, char *dir)
{

    strcpy(prompt, "Локација:");
    strncat(prompt, update_location(get_inode("."), dir, BUFSIZ), BUFSIZ - strlen(prompt) - 1);
    return prompt;
}

void fatal(char *s1, char *s2, int n)
{
    fprintf(stderr, "Error: %s,%s\n", s1, s2);
    exit(n);
}

/*
 * Function to return username string for the purpose of constructing the default terminal prompt.
 * If pwuid is obtained, returns username, and othervise returns "Aloha!" as a default prompt.
 */
char *get_username()
{
    struct passwd *pw_ptr;
    static char *greeting;

    if ((pw_ptr = getpwuid(getuid())) == NULL)
    {
        greeting = "Aloha!";
        return greeting;
    }
    return pw_ptr->pw_name;
}

char *update_location(ino_t target_inode, char *current_dir, int buflen)
{

    DIR *dir_ptr;
    struct dirent *direntp;

    dir_ptr = opendir("..");
    if (dir_ptr == NULL)
    {
        perror(".");
        exit(1);
    }

    while ((direntp = readdir(dir_ptr)) != NULL)
    {
        if (direntp->d_ino == target_inode)
        {
            strncpy(current_dir, "/", BUFSIZ - 1);
            strncat(current_dir, direntp->d_name, BUFSIZ - strlen(current_dir) - 1);
            strncat(current_dir, "/ >> ", BUFSIZ - strlen(current_dir) - 1);
            closedir(dir_ptr);
        }
    }
    return current_dir;
}

/*
 * Returns inode number of the file. Taken from the book Understanding Unox/Linux Programming: A Guide to
 * Theory and Practice written by Bruce Molay.
 */
ino_t get_inode(char *fname)
{
    struct stat info;

    if (stat(fname, &info) == -1)
    {
        fprintf(stderr, "Cannot stat");
        perror(fname);
        exit(1);
    }
    return info.st_ino;
}
