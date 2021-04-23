#include <stdio.h>
#include <stdlib.h> 
#include "commando.h"

void cmdcol_add(cmdcol_t *col, cmd_t *cmd){
    
    col->cmd[col->size] = cmd; // Add cmd to the cmdcol struct
    col->size++; // Increment cmdcol size

    if (col->size > MAX_CMDS){ 
        printf("Error, max commands exceeded");
    }
    
}


void cmdcol_print(cmdcol_t *col){


    printf("%-4s ", "JOB");     // Format printing of job list
    printf("#%-8s ", "PID");
    printf("%4s ", "STAT");
    printf("%10s ", "STR_STAT");
    printf("%4s ", "OUTB");
    printf("%-s\n", "COMMAND");

    for(int i = 0; i < col->size; i++) {    // Iterate through job list and prints   
        printf("%-4d ", i);
        printf("#%-8d ", (col->cmd[i])->pid);
        printf("%4d ", (col->cmd[i])->status);
        printf("%10s ", (col->cmd[i])->str_status);
        printf("%4d ", (col->cmd[i])->output_size);

        for(int j = 0; j < sizeof(col->cmd[i]->argv); j++) {
            if (col->cmd[i]->argv[j]!= NULL) {
                printf("%-s ", (col->cmd[i]->argv[j]));
            }
            else {
                printf("\n");
                break;
            }
        
        }
    }
}


void cmdcol_update_state(cmdcol_t *col, int block){ // Updates state of each cmd according to input block status
    for (int i = 0; i < col->size; i++) {
        cmd_update_state(col->cmd[i], block); 
    }
}


void cmdcol_freeall(cmdcol_t *col){ 
    for (int i = 0; i < col->size; i++) {
        cmd_free(col->cmd[i]); // Frees each cmd in col
    }
}

