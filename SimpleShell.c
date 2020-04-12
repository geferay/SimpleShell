#include <stdio.h>   //standard input/output library
#include <stdlib.h>
#include <unistd.h>  //unix standard library
#include <assert.h>  //assert error-handling
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE 80 /* The maximum length command */

int main(void)
{
    int argsMAX = MAX_LINE/2 + 1;
    char *args[argsMAX]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    char command[MAX_LINE] = "";
    char history[MAX_LINE] = "";
    const char* exCom = "exit";
    size_t cLength = 0;
    size_t maxLine = MAX_LINE;
    
    /*clean junk from memory*/
    for(int c = 0; c < MAX_LINE; ++c){
        command[c] = '\0';
        history[c] = '\0';
    }
    for(int a = 0; a < argsMAX; ++a){
        args[a] = NULL;
    }
    
    while (should_run) {
        printf("osh>");
        fflush(stdout);
        
        /*Get the command.*/
        {
            char *com = command;
            cLength = getline(&com,&maxLine,stdin);
            command[cLength] = '\0'; /*make sure no junk got to end*/
            printf("Received input: %s", command);
            fflush(stdout);
        }
        {
            args[0] = (char*)malloc(MAX_LINE * sizeof(char));
            int i = 0;
            int a = 0;
            int c = 0;
            for( ; a < argsMAX && c < MAX_LINE; ++i, ++c){
                if(command[c] == ' '){
                    args[a][i] = '\0'; /*end current string*/
                    i = 0;
                    ++a; /*go to next arg*/
                    ++c; /*skip the space*/
                    args[a] = (char*)malloc(MAX_LINE * sizeof(char));
                }
                args[a][i] = command[c];
            }
        }
        for(int i = 0; i < argsMAX; ++i){
            if(args[i] != NULL){
                printf("args[%d] contains: %s\n", i, args[i]);
                fflush(stdout);
            }
        }
        
//        printf("You entered: "); fflush(stdout);
//        for(int i = 0; command[i] != '\0' && i < MAX_LINE; ++i){
//            printf("%c", command[i]);
//        }
//        printf("\n"); fflush(stdout);
        
        //crudely check for exit command
        if(command[0] == 'e' && command[1] == 'x' && command[2] == 'i'
           && command[3] == 't'/* && command[4] == '\0'*/){
            should_run = 0;
        }
        else{
            //            char* args[3];
            //
            //            //set args[0] to command
            //            args[0] = (char*)malloc(MAX_LINE * sizeof(char));
            //            args[0] = "ps";
            //            char* args0 = args[0];
            //
            //            //set args[1] to parameters
            //            args[1] = (char*)malloc(MAX_LINE * sizeof(char));
            //            args[1] = "-ael";
            //            char* args1 = args[1];
            //            //set args[2] to NULL
            //            args[2] = NULL;
            //            //separate command from arguments
            //            int commandPlace = 0;    /*tracks where we are in the command string*/
            //            for(int i = 0; command[commandPlace] != ' '; ++i, ++commandPlace){
            ////                printf("We are here"); fflush(stdout);
            //                //                args0[i] = command[commandPlace];
            //            }
            //            ++commandPlace;
            //            for(int i = 0; command[commandPlace] != '\0'; ++i, ++commandPlace){
            //                args1[i] = command[commandPlace];
            //            }
            //
            //            //The user would like to run a command.
            //            pid_t pid = fork();
            //            assert(pid >= 0);  //This is bad.
            //            if(pid == 0){
            //                //pid == 0 means we're running as the child.
            //                execvp(args[0], args);
            //                printf("Should never reach here! \n");
            //            }
            //            else{
            //                //pid > 0 means we're running as the parent.
            //                printf("Launch pid = %d \n", pid);
            //                wait(NULL);
            //            }
            //
            //            /**
            //             * After reading user input, the steps are:
            //             * (1) fork a child process using fork()
            //             * (2) the child process will invoke execvp()
            //             * (3) parent will invoke wait() unless command included &
            //             */
            //
            //            //free the memory
            //            free(args[0]);
            //            free(args[1]);
            //            //    free(args[2]);
        }
        
        for(int a = 0; a < argsMAX; ++a){
            if(args[a] != NULL){
                free(args[a]);
                args[a] = NULL;
            }
        }
    }
    return 0;
}
