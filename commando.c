#include <stdio.h>
#include <stdlib.h> //don't know if we need this
#include "commando.h"

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); //turn off output buffering

    char* result[MAX_LINE];
    printf("@>");

    while(result != NULL){
        fgets(*result, MAX_LINE, stdin);
        parse_into_tokens(result, char *tokens[], int *ntok); //what is tokens[] and ntok?
        if(ntok == 0){
            break; //User hits enter ?
        }
        if(strncmp(argv[0],'--echo')==0 || getenv("COMMAND_ECHO")!= NULL){ //How to check echoing?
            setenv("COMMAND_ECHO", "ON",1); 
        }
        if(strncmp(tokens[0], 'help')==0){
            
        }
        else if(strncmp(tokens[0], 'list')==0){

        }
        else if(strncmp(tokens[0], 'exit')==0){

        }
        else if(strncmp(tokens[0], 'pause')==0){

        }
        else if(strncmp(tokens[0], 'output-for')==0){

        }
        else if(strncmp(tokens[0], 'output-all')==0){
        
        }
        else if(strncmp(tokens[0], 'wait-for')==0){

        }
        else if(strncmp(tokens[0], 'wait-all')==0){

        }
        else{

        }
        cmd_t cmd1 = cmd_new(tokens[]); 
        cmd_start(cmd1);
    }
    
    

   //didnt finish this
        printf("End of input\n");
        break;
    }



}