/* execute.c - code used by small shell to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <termios.h>
#include "smsh.h"

extern int bgd_processes;

int execute(char *argv[])
/*                                                                                                                                                                           
 * purpose: run a program passing it arguments                                                                                                                               
 * returns: status returned via wait, or -1 on error                                                                                                                         
 *  errors: -1 on fork() or wait() errors                                                                                                                                    
 */
{
  int   child;
  int   child_info = -1;
  int   run_in_background = NO;
  if ( argv[0] == NULL ){               /* nothing succeeds     */
    return 0;
  }
  int   elem_num = get_args_num(argv); //element count                                                                                                                       
  char  **argv_trimmed;
  if (strcmp(argv[elem_num],"&") == 0){//here we compare the last element                                                                                                    
        run_in_background = YES;
        argv_trimmed = (char**)malloc(BUFSIZ);
        trim_array(argv,argv_trimmed, elem_num-1);
   }

  if ( (child = fork())  == -1 ){
        perror("fork");

  }else if ( child == 0 ){//child                                                                                                                                            
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

                        if(run_in_background) {
                        int gid;
if ( (gid = setpgid(0,0)) == -1) { // move child to a different group                                                                                
                        perror("setpgid");
                        }
                        bgd_processes++;
                        printf("Process %d started in the background.\n",getpid());
                        execvp(argv_trimmed[0], argv_trimmed);
                        }else {
                        execvp(argv[0], argv);
                        }
                        perror("cannot execute command");
                        exit(1);


}else { //parent                                                                                                                                                           
        //wait for foreground process to terminate                                                                                                                           
        if (run_in_background == NO) {
        if ( waitpid(child,&child_info,0) == -1 ) {
                perror("Wait");
        }
        }else {
        tcsetpgrp(STDOUT_FILENO, getpid());
        setpgid(child,child); //put a child in a new group                                                                                                                   
        }
        int pid;
        for (int i = 0; i <= bgd_processes; i++) {
        pid = waitpid (-1, &child_info, WNOHANG);
        if (pid == 0) {
        printf("Process is still running.\n");
        }else if( (pid > 0) && WIFEXITED(child_info)) {
        printf("Process %d terminated.\n", pid);
        bgd_processes--;
        }
        sleep(1);

        }                                                                                                                                    

    }//parent exit                                                                                                                                                           
  return child_info;
}

/*                                                                                                                                                                           
 * Takes a pointer to char array and the number of elements to be copied. Copies elements to the                                                                             
 * helper and returns modified array.                                                                                                                                        
 */
char **trim_array(char **argv,char **replacement, int n) {


        for (int i = 0; i <= n; i++) {
                replacement[i] = argv[i];
        }

        return replacement;
}

/*                                                                                                                                                                           
 * Loops the array of char** and determine the number of strings. Returns the last index of the array.                                                                       
 */
int get_args_num(char **argv) {
        int i= 0;
        while (argv[i] != NULL) {
        i++;
        }
 return i-1;
}

/*                                                                                                                                                                           
 * Handler to catch SIGCHLD, reports the status of the signalling process.                                                                                                   
 */
void handle_child(int sig) {
        pid_t pid;
        int status;

        if( (pid = waitpid(-1,&status, WNOHANG)) != -1) {
        if ((pid > 0) && WIFEXITED(status)){
        printf("Process %d is terminated.\n", pid);
        }
        }//while                                                                                                                                                             
}
