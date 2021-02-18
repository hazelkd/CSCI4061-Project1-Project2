#include <stdio.h>
#include <stdlib.h> //don't know if we need this
#include "commando.h"

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); //turn off output buffering

    char* result[MAX_LINE];
    char *tokens[MAX_LINE]; // is it ok to declare here?//WHAT SIZE?
    int *ntok;
    cmdcol_t *col;

    printf("@>");

    while(result != NULL){
        fgets(*result, MAX_LINE, stdin);
        parse_into_tokens(result, tokens, ntok); //what is tokens[] and ntok?
        if(ntok == 0){
            break; //User hits enter ?
        }
        if(strcmp(argv[0],"--echo")==0 || getenv("COMMAND_ECHO")!= NULL){ //How to check echoing?
            setenv("COMMAND_ECHO", "ON",1); 
        }
        if(strcmp(tokens[0], 'help')==0){ // i changed to strcmp instead of strncmp cause then we don't need length
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

        }
        else if(strcmp(tokens[0], 'list')==0){
            cmdcol_print(col);
        }
        else if(strcmp(tokens[0], 'exit')==0){
            exit;
        }
        else if(strcmp(tokens[0], 'pause')==0){

        }
        else if(strcmp(tokens[0], 'output-for')==0){
            int jobNum = atoi(tokens[2]);
            cmd_fetch_output(col->cmd[jobNum]);
        }
        else if(strcmp(tokens[0], 'output-all')==0){
            for (int i = 0; i < col->size; i++){
                cmd_fetch_output(col->cmd[i]);
            }
        }
        else if(strcmp(tokens[0], 'wait-for')==0){
            int jobNum = atoi(tokens[2]);
            cmd_update_state((col->cmd[jobNum]), DOBLOCK);
        }
        else if(strcmp(tokens[0], 'wait-all')==0){
            for (int i = 0; i < col->size; i++) {
                cmd_update_state((col->cmd[i]), DOBLOCK);
            }
        }
        else{
            cmd_t *cmd1 = cmd_new(tokens); 
            //have to add the new cmd to cmdcol_t and start it running 
            cmdcol_add(col, cmd1);
            cmd_start(cmd1);
        }
    }
    
    

   //didnt finish this
        printf("End of input\n");
        cmdcol_update_state(col, NOBLOCK);
        //break;
    }



}