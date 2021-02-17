#include <stdio.h>
#include <stdlib.h> //don't know if we need this
#include "commando.h"

int main() {
    setvbuf(stdout, NULL, _IONBF, 0); //turn off output buffering

    char* result[MAX_LINE];
    printf("@>");

    while(result != NULL){
        fgets(*result, MAX_LINE, stdin);
        parse_into_tokens(char input_command[], char *tokens[], int *ntok);

    }
    //if(echoing is enabled){
        //Echo (print)
    //}
    

   //didnt finish this
        printf("End of input\n");
        break;
    }



}