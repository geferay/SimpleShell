#include <stdio.h>   //standard input/output library
#include <stdlib.h>
#include <unistd.h>  //unix standard library
#include <assert.h>  //assert error-handling
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE 80 /* The maximum length command */
#define DIAG_OUTPUT 0

const int argsMAX = MAX_LINE/2 + 1;

enum redir_t {NONE, TO_FILE, FROM_FILE, PIPE};
int tokenizeCommand(char* input, char* args[]);
char* redirName(enum redir_t r);

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
            char command[MAX_LINE] = "";
            size_t cLength = 0;
            size_t maxLine = MAX_LINE;
            for(int a = 0; a < argsMAX; ++a){
                args[a] = NULL;
            }
            /*clean junk from memory*/
            for(int c = 0; c < MAX_LINE; ++c){
                input[c] = '\0';
                command[c] = '\0';
            }
            
            printf("osh>");
            fflush(stdout);
            
            /*Get the input.*/
            {
                char *com = input;
                cLength = getline(&com,&maxLine,stdin);
                input[cLength] = '\0'; /*make sure no junk got to end*/
                if(DIAG_OUTPUT) printf("Received input: %s", input);
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
            
            
            //crudely check for exit command
            if(input[0] == 'e' && input[1] == 'x' && input[2] == 'i'
               && input[3] == 't'/* && input[4] == '\0'*/){
                should_run = 0;
            }
            
            //            int commandsLeft = 1;
            int l = 0; /*l tracks location in input scan*/
            while(input[l] != '\0' && should_run == 1 && l < MAX_LINE){
                if(DIAG_OUTPUT){
                    printf("=====================DIAGNOSTICS=====================\n");
                    assert(input[l] != '\0');
                    printf("input[%d] is: %c\n", l, input[l]); fflush(stdout);
                    assert(should_run == 1);
                    printf("should_run is: %d\n", should_run); fflush(stdout);
                    assert(l < MAX_LINE);
                    printf("l is: %d\n", l); fflush(stdout);
                    printf("===================END DIAGNOSTICS===================\n");
                }
                
                int waitForChild = 1; /*rmember if we shall wait for child*/
                int haveCommand = 0;  /*be certain about whether we have command or not*/
                
                /*free() memory*/
                for(int a = 0; a < argsMAX; ++a){
                    if(args[a] != NULL){
                        free(args[a]);
                        args[a] = NULL;
                        if(args[a] == NULL){
                            if(DIAG_OUTPUT) printf("args[%d] freed\n", a);
                            if(DIAG_OUTPUT) fflush(stdout);
                        }
                    }
                }
                for(int i = 0; i < MAX_LINE; ++i){
                    command[i] = '\0';
                }
                
                /*single out and run individual commands separated by '&' and ';';
                 Inputs without these are assumed to be 1 command ending with ';'.*/
                {
                    int p = 0;
                    /*skip starting spaces*/
                    for( ; l < MAX_LINE
                        && (input[l] < '!'
                            || input[l] > '~'); ++l);
                    //                    /*brute-force leave loop when at end*/
                    //                    if(l >= MAX_LINE){
                    //                        break;
                    //                    }
                    /*copy next command from input*/
                    for( ;
                        l < MAX_LINE
                        && input[l] != '\0'
                        && input[l] != '&'
                        && input[l] != ';'
                        ; ++l, ++p){
                        command[p] = input[l];
                        haveCommand = 1; /*if we're here, we should haveCommand*/
                    }
                    if(input[l] == '&'){
                        waitForChild = 0; /*don't wait for child, if received '&'*/
                        ++l; /*got to next l, or we'll have endless loop*/
                    }
                    if(input[l] == ';'){
                        waitForChild = 1; /*don't wait for child, if received '&'*/
                        ++l; /*got to next l, or we'll have endless loop*/
                    }
                    /*get rid of ending spaces*/
                    for(--p; p >=0; --p){
                        if(command[p] >= '!' && command[p] <= '~'){
                            ++p;
                            break;
                        }
                    }
                    /*properly end command string*/
                    if(p < MAX_LINE){
                        command[p] = '\0';
                        ++p;
                    }
                    /*pad remaing string with '\0'*/
                    for( ; p < MAX_LINE; ++p){
                        command[p] = '\0';
                    }
                    //                    printf("Got command: %s\n", command); fflush(stdout);
                    //                    printf("Wait on child?: %s\n", waitForChild? "TRUE":"FALSE");
                }
                if(DIAG_OUTPUT) printf("Command is: %s\n", command);
                if(DIAG_OUTPUT) printf("Wait on child?: %s\n", waitForChild? "TRUE":"FALSE");
                
                /*This function splits the command up.*/
                tokenizeCommand(command, args);
                
                for(int i = 0; i < argsMAX; ++i){
                    if(args[i] != NULL){
                        if(DIAG_OUTPUT) printf("args[%d] contains: %s\n", i, args[i]);
                        if(DIAG_OUTPUT) fflush(stdout);
                    }
                }
                if(DIAG_OUTPUT) fflush(stdout); /*exit(0);*/
                
                if(haveCommand == 1){
                    /*check for redirection*/
                    enum redir_t redirectionType;
                    if(args[1] != NULL){
                        switch(args[1][0]){
                            case '>': redirectionType = TO_FILE; break;
                            case '<': redirectionType = FROM_FILE; break;
                            case '|': redirectionType = PIPE; break;
                            default: redirectionType = NONE; break;
                        }
                    }
                    else{
                        redirectionType = NONE;
                    }
                    if(DIAG_OUTPUT) printf("<<<<<<<<<<<<<<<<<<<<<<<<WE ARE HERE>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
                    if(DIAG_OUTPUT) fflush(stdout); /*exit(0);*/
                    if(DIAG_OUTPUT) printf("redirection type is: %s\n", redirName(redirectionType));
                    if(DIAG_OUTPUT) fflush(stdout);
                    
                    //The user would like to run a command.
                    pid_t pid = fork();
                    assert(pid >= 0);  /*This would be bad, to fail.*/
                    if(DIAG_OUTPUT) {printf("We are here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s\n", (pid == 0)? "CHILD":"PARENT"); fflush(stdout);}
//                    int fileFD = -1;
                    if(pid == 0){
                        //pid == 0 means we're running as the child.
                        if(DIAG_OUTPUT) {printf("######################RUNNING COMMAND######################\n");
                            printf("######################%s\n", command);
                            fflush(stdout);}
                        switch (redirectionType) {
                            case TO_FILE:{
                                if(DIAG_OUTPUT) {printf("##########################TO FILE##########################\n");
                                    fflush(stdout);}
                                {
                                    FILE *fp;
                                    fp = fopen(args[2], "w+");
                                    int fileFD = fileno(fp); /*get the file descriptor*/
                                    dup2(fileFD, 1); /*redirect stdout to file*/
                                    char* newArgs[2];
                                    newArgs[0] = args[0];
                                    newArgs[1] = NULL;
                                    pid_t grandChild = fork();
                                    if(grandChild == 0){
                                        /*Do file stuff*/
                                        /*build new args*/
                                        execvp(newArgs[0], newArgs);
                                    }
                                    else{
                                        /*This parent (child of original parent) always waits for its children.*/
                                        wait(NULL);
                                        /*CLOSE FILE*/
                                        fclose(fp);
                                    }
                                }
                            }
                                break;
                            case FROM_FILE:{
                                if(DIAG_OUTPUT) {printf("#########################FROM FILE#########################\n");
                                    fflush(stdout);}
                                {
                                    FILE *fp;
                                    fp = fopen(args[2], "r");
                                    int fileFD = fileno(fp); /*get the file descriptor*/
                                    dup2(fileFD, 0); /*redirect stdout to file*/
                                    char* newArgs[2];
                                    newArgs[0] = args[0];
                                    newArgs[1] = NULL;
                                    pid_t grandChild = fork();
                                    if(grandChild == 0){
                                        /*Do file stuff*/
                                        /*build new args*/
                                        execvp(newArgs[0], newArgs);
                                    }
                                    else{
                                        /*This parent (child of original parent) always waits for its children.*/
                                        wait(NULL);
                                        /*CLOSE FILE*/
                                        fclose(fp);
                                    }
                                }
                            }
                                break;
                            case PIPE:{
                                if(DIAG_OUTPUT) {printf("############################PIPE###########################\n");
                                    fflush(stdout);}
                                {
                                    int fd[2];
                                    pipe(fd);
                                    pid_t grandChild = fork();
                                    
                                    if(grandChild == 0){
                                        close(fd[1]); /*aim't going to write*/
                                        dup2(fd[0], 0); /*fd[0] <= sysin?*/
                                        char* newArgs[2];
                                        newArgs[0] = args[2];
                                        newArgs[1] = NULL;
                                        execvp(newArgs[0], newArgs);
                                        exit(0);
                                    }
                                    else{
                                        close(fd[0]); /*aim't going to read*/
                                        dup2(fd[1], 1); /*sysout => fd[1]*/
                                        char* newArgs[2];
                                        newArgs[0] = args[0];
                                        newArgs[1] = NULL;
                                        execvp(newArgs[0], newArgs);
                                        exit(0);
                                    }
                                }
                            }
                                break;
                                
                            default:{
                                if(DIAG_OUTPUT) {printf("############################NONE###########################\n");
                                    fflush(stdout);}
                                execvp(args[0], args);
                                printf("Should never reach here! \n");
                                printf("Your command was likely invalid or not found.\n");
                                fflush(stdout);
                            }
                                break;
                        }
                        exit(0); /*we need to close the child process*/
                    }
                    else{
                        //pid > 0 means we're running as the parent.
                        //                printf("Launch pid = %d \n", pid);
                        if(waitForChild == 1){
                            wait(NULL);
                        }
                    }
                }
                
                //                exit(0);
            }
            /*free() memory*/
            for(int a = 0; a < argsMAX; ++a){
                if(args[a] != NULL){
                    free(args[a]);
                    args[a] = NULL;
                    if(args[a] == NULL){
                        if(DIAG_OUTPUT) printf("args[%d] freed\n", a);
                        if(DIAG_OUTPUT) fflush(stdout);
                    }
                }
            }
        }
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

char* redirName(enum redir_t r){
    
    switch(r){
        case NONE: return "NONE";
        case TO_FILE: return "TO_FILE";
        case FROM_FILE: return "FROM_FILE";
        case PIPE: return "PIPE";
        default: return "UNKNOWN";
    }
}
