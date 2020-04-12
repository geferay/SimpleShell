#include <stdio.h>   //standard input/output library
#include <stdlib.h>
#include <unistd.h>  //unix standard library
#include <assert.h>  //assert error-handling
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE 80 /* The maximum length command */
const int argsMAX = MAX_LINE/2 + 1;

int tokenizeCommand(char* input, char* args[]);

int main(void)
{
    int should_run = 1; /* flag to determine when to exit program */
    char history[MAX_LINE] = "";
    
    /*clean junk from memory*/
    for(int c = 0; c < MAX_LINE; ++c){
        history[c] = '\0';
    }
    
    while (should_run) {
        {
            /*sorry way to erase the stack, because I was getting artifacts*/
            /*it did get rid of the artifacts*/
            char blankingBuffer[1000000];
            for(int i = 0; i < 1000000; ++i){
                blankingBuffer[i] = '\0';
            }
        }
        {
            char *args[argsMAX]; /* command line arguments */
            char input[MAX_LINE] = "";
            char* command = NULL;
            size_t cLength = 0;
            size_t maxLine = MAX_LINE;
            for(int a = 0; a < argsMAX; ++a){
                args[a] = NULL;
            }
            /*clean junk from memory*/
            for(int c = 0; c < MAX_LINE; ++c){
                input[c] = '\0';
            }
            
            printf("osh>");
            fflush(stdout);
            
            /*Get the input.*/
            {
                char *com = input;
                cLength = getline(&com,&maxLine,stdin);
                input[cLength] = '\0'; /*make sure no junk got to end*/
                printf("Received input: %s", input);
                fflush(stdout);
            }
            
            if(input[0] == '!' && input[1] == '!'){
                if(history[0] == '\0'){
                    printf("ERROR: NO COMMANDS IN HISTORY");
                } else {
                    /*remember history*/
                    for(int c = 0; c < MAX_LINE; ++c){
                        input[c] = history[c];
                    }
                    printf("Re-Running: %s\n", input);
                }
            }
            else{
                /*forget history*/
                /*record for history*/
                for(int c = 0; c < MAX_LINE; ++c){
                    history[c] = input[c];
                }
            }
            
            //            for(int l = 0; l < MAX_LINE; ++l){
            /*This function splits the command up.*/
            tokenizeCommand(input, args);
            
            for(int i = 0; i < argsMAX; ++i){
                if(args[i] != NULL){
                    printf("args[%d] contains: %s\n", i, args[i]);
                    fflush(stdout);
                }
            }
            
            //crudely check for exit command
            if(input[0] == 'e' && input[1] == 'x' && input[2] == 'i'
               && input[3] == 't'/* && input[4] == '\0'*/){
                should_run = 0;
            }
            else{
                //The user would like to run a command.
                pid_t pid = fork();
                assert(pid >= 0);  /*This would be bad, to fail.*/
                if(pid == 0){
                    //pid == 0 means we're running as the child.
                    execvp(args[0], args);
                    printf("Should never reach here! \n");
                    exit(-1); /*we do sometimes get here, so exit with error*/
                }
                else{
                    //pid > 0 means we're running as the parent.
                    //                printf("Launch pid = %d \n", pid);
                    wait(NULL);
                }
                
                /**
                 * After reading user input, the steps are:
                 * (1) fork a child process using fork()
                 * (2) the child process will invoke execvp()
                 * (3) parent will invoke wait() unless command included &
                 */
            }
            
            for(int a = 0; a < argsMAX; ++a){
                if(args[a] != NULL){
                    free(args[a]);
                    args[a] = NULL;
                    if(args[a] == NULL){
                        printf("args[%d] freed\n", a);
                        fflush(stdout);
                    }
                }
            }
        }
        //        }
    }
    return 0;
}


int tokenizeCommand(char* input, char* args[]){
    {
        /*sorry way to erase the stack, because I was getting artifacts*/
        /*it did get rid of the artifacts*/
        char blankingBuffer[1000000];
        for(int i = 0; i < 1000000; ++i){
            blankingBuffer[i] = '\0';
        }
    }
    args[0] = (char*)malloc(MAX_LINE * sizeof(char));
    for(int d = 0; d < MAX_LINE; ++d){
        args[0][d] = '\0';
    }
    int c = 0;
    int i = 0;
    int a = 0;
    for( ; a < argsMAX && c < MAX_LINE; ++i, ++c){
        if(input[c] == ' '){
            args[a][i] = '\0'; /*end current string*/
            i = 0;
            ++a; /*go to next arg*/
            ++c; /*skip the space*/
            args[a] = (char*)malloc(MAX_LINE * sizeof(char));
            for(int d = 0; d < MAX_LINE; ++d){
                args[a][d] = '\0';
            }
        }
        if(input[c] >= '!' && input[c] <= '~'){
            args[a][i] = input[c];
        }
    }
    return a + 1;
}
