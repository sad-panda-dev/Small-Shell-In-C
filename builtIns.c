#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "smsh.h"

void exit_shell(char ** arglist) {

        if (arglist[1]){
        exit(atoi(arglist[1]));
        }else {
        exit(0);
        }

}

void cd(char **arglist){

        if(arglist[1]){
        chdir(arglist[1]);
        }else {
        chdir(getenv("HOME"));
        }

}
