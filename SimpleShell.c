#include <stdio.h>   //standard input/output library
#include <unistd.h>  //unix standard library
#include <assert.h>  //assert error-handling
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    char command[MAX_LINE] = "";
    char history[MAX_LINE] = "";
    
    while (should_run) {
//        int no_input = 1;
//        while(no_input){
            printf("osh>");
            fflush(stdout);
            
            /*Get the command.*/
            scanf("%s", &command);
//            printf("%x%x", command[0], command[0]);
//            if(command[0] == '\0') no_input = 1; /*empty is bad*/
//            else                   no_input = 0; /*full  is good*/
//        }
        
        
//        if(command[0] == '!' && command[1] == '!' && command[2] == '\0'){
//            if(history[0] != '\0'){
//                /*The user wants to travel back in time.*/
//                printf("\b\b%s", history);
//
//                /*copy history into command - crudely*/
//                for(int i = 0; history[i] && history[i] < MAX_LINE; ++i){
//                    command[i] = history[i];
//                }
//
//                //But, NOTE, we do not stop execution. We just run this historic command.*/
//            }
//            else{
//                printf("ERROR: You are at the beginning of time, and history is yet to come!");
//                continue;
//            }
//        }
        
//        /*copy command into history - crudely*/
//        for(int i = 0; command[i] && command[i] < MAX_LINE; ++i){
//            history[i] = command[i];
//        }
        
        //crudely check for exit command
        if(command[0] == 'e' && command[1] == 'x' && command[2] == 'i'
           && command[3] == 't' && command[4] == '\0'){
            //The user wants us to exit.
            should_run = 0;
        }
        else{
            //The user would like to run a command.
            pid_t pid = fork();
            assert(pid >= 0);  //This is bad.
            if(pid == 0){
                //pid == 0 means we're running as the child.
                execlp(command, "ls", NULL);
                printf("Should never reach here! \n");
            }
            else{
                //pid > 0 means we're running as the parent.
                printf("Launch pid = %d \n", pid);
                wait(NULL);
            }
            
            /**
             * After reading user input, the steps are:
             * (1) fork a child process using fork()
             * (2) the child process will invoke execvp()
             * (3) parent will invoke wait() unless command included &
             */
        }
    }
    return 0;
}
