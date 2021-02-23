#include <stdio.h>
#include <stdlib.h> 
#include "commando.h"


int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); //turn off output buffering

    char* result[MAX_LINE];
    char result[MAX_LINE] = {0};
    char *tokens[MAX_LINE]; 
    int *ntok=0;
    cmdcol_t *col= NULL;
    cmd_t *cmd = cmd_new(tokens);
    cmdcol_add(col,cmd);
    int echo =0;

    //printf("@>");
    fgets(*result, MAX_LINE, stdin);
    parse_into_tokens(*result, tokens, ntok); 

    int ntok=0;
    char *backup=NULL;
    int echo =0;
    cmdcol_t *col= malloc(sizeof(cmdcol_t)); // Allocate an initial pointer to memory;

    // if(strcmp(argv[1],"--echo")==0 || getenv("COMMAND_ECHO")!= NULL){ // Check for echoing
    //      setenv("COMMAND_ECHO", "1",1);
    //    echo = 1;
    //}
    while(result != NULL){   // Checking for end of input
            if(ntok == 0){ // User hits enter 
                break;}
    }                       

    // backup = fgets(result, MAX_LINE, stdin);

    while(1){               // Checking for end of input
            if(backup == NULL){   // User hits enter 
                break; 
            }

            printf("@>"); 

            parse_into_tokens(result, tokens, &ntok);
            if(echo){       // If echoing is on, print command
                printf(" %s\n",tokens[0]);
            }

            if(strcmp(tokens[0], "help")==0){  // Print help options
               printf("COMMANDO COMMANDS \n");
                printf("%-20s", "help"); printf(": show this message\n");
                printf("%-20s", "exit"); printf(": exit the program\n");
                printf("%-20s", "list"); printf(": list all jobs that have been started giving information on each\n");
                printf("%-20s", "pause nanos secs"); printf(": pause for the given number of nanseconds and seconds\n");
                printf("%-20s", "output-for int"); printf(": print the output for given job number\n");
                printf("%-20s", "output-all"); printf(": print output for all jobs\n");
                printf("%-20s", "wait-for int"); printf(": wait until the given job number finishes\n");
                printf("%-20s", "wait-all"); printf(": wait for all jobs to finish\n");
                printf("%-20s", "command arg1 ..."); printf(": non built-in is run as a job\n");
                printf("%-19s", "command arg1 ..."); printf(": non-built-in is run as a job\n");

            }
           
            else if(strcmp(tokens[0], "list")==0){ // Print current job list

                cmdcol_print(col);

            }
         
            else if(strcmp(tokens[0], "exit")==0){  // Exits program
                exit(1); //added 1
            }
          
            else if(strcmp(tokens[0], "pause")==0){ // Pauses program for specified time
                long nanos = atoi(tokens[2]);
                int secs = atoi(tokens[3]);
                pause_for(nanos, secs);
            }
           
            else if(strcmp(tokens[0], "output-for")==0){ // Outputs for given job number
                int jobNum = atoi(tokens[2]);
                cmd_fetch_output(col->cmd[jobNum]);
            }
          
            else if(strcmp(tokens[0], "output-all")==0){ // Outputs all jobs
                for (int i = 0; i < col->size; i++){
                    cmd_fetch_output(col->cmd[i]);
                }
            }
           
            else if(strcmp(tokens[0], "wait-for")==0){ // Waits for specified job number
                int jobNum = atoi(tokens[2]);
                cmd_update_state((col->cmd[jobNum]), DOBLOCK);
            }
            
            else if(strcmp(tokens[0],"wait-all")==0){ // Waits for all jobs
                for (int i = 0; i < col->size; i++) {
                    cmd_update_state((col->cmd[i]), DOBLOCK);
                }
            }
            else{
               
                cmd_t *cmd1 = cmd_new(tokens); // Create a new command if not one of previous recognized ones

                cmdcol_add(col, cmd1); 
                cmd_start(cmd1);
            }
         
            backup = fgets(result, MAX_LINE, stdin); 
    }


        free(*tokens);
        printf("End of input\n");
        cmdcol_update_state(col, NOBLOCK);
        //break; - do we need this? 
    cmdcol_freeall(col);
        cmdcol_update_state(col, NOBLOCK); 

    cmdcol_freeall(col); // Free all commands in col
            }
    