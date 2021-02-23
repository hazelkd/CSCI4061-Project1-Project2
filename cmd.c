#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "commando.h"

cmd_t *cmd_new(char *argv[]){
    
    cmd_t *cmd = malloc(sizeof(cmd_t)); // Allocate an initial pointer to memory
    *cmd->name = '\0';

     for(int i=0; i < ARG_MAX; i++){
        if(argv[i] == NULL){  // If no more arguments, break
             cmd->argv[i]= NULL;
             break;
        }
        else{
            cmd->argv[i] = strdup(argv[i]); // Take argv input and put it in cmd
        }
     }
   
    if (argv[0] == NULL) { 
        *cmd->name = '\0';
    }
    else {
        strncpy(cmd->name,argv[0],NAME_MAX); // Take name of command input in argv and assign to name
    }

 
    cmd->finished = 0;            // Initialize all fields of new cmd
    snprintf(cmd->str_status, STATUS_LEN, "INIT");  
    cmd->status = -1;
    cmd->output = NULL;
    cmd->output_size = -1;
    cmd->pid = -1; 
    cmd->out_pipe[0] = -1;
    cmd->out_pipe[1] = -1;            

    return cmd;
    cmd_free(cmd); // Free the cmd structure


}

void cmd_free(cmd_t *cmd){      //free cmd
    for(int i = 0; i <= ARG_MAX+1; i++) {       //free cmd->argv elements
        if(cmd->argv[i]==NULL){
            break;
        }
        free(cmd->argv[i]);
    }

    if (cmd->output != NULL) {          //free cmd output
        free(cmd->output);
    }

    free(cmd);          //free cmd itself

}

void cmd_start(cmd_t *cmd){
     
    snprintf(cmd->str_status, STATUS_LEN, "RUN");   // Change status to "RUN"
    
    pipe(cmd->out_pipe);   // Create an out-pipe for parent/child communication

    pid_t child_pid = fork();
 
    if(child_pid == 0 ){           
       int backup = dup(STDOUT_FILENO);     // Preserve STDOUT by storing in backup to restore later
       dup2(cmd->out_pipe[PWRITE],STDOUT_FILENO); 
        
        execvp(cmd->name, cmd->argv);    // Create a new memory image and run the command
        dup2(backup,STDOUT_FILENO);     // Restore STDOUT
        close(cmd->out_pipe[PREAD]);
    }

    else{
        cmd->pid = child_pid;       // Assign the child pid to the parent
        close(cmd->out_pipe[PWRITE]); // Close write end of pipe
    }
    
}
void cmd_update_state(cmd_t *cmd, int block){

    pid_t pid = cmd->pid;
    int status=-11111; // Initialize status to unreachable value

    while (cmd->finished != 1){
      
        int ret = waitpid(cmd->pid, &status, block); // Make parent wait for child with the block argument that was passed in
        
        if(ret == -1){
            perror("waitpid() failed\n");
        }
        else if (ret == 0){ 
            printf("Pid process has not completed");
        }
        else if(ret == pid){

            if (WIFEXITED(status)){  // When child finishes...
                
                cmd->finished = 1;
                cmd->status = WEXITSTATUS(status); // Update status of command to the exit status 
                if (cmd->status == 0) {
                    strcpy(cmd->str_status, "EXIT(0)"); // Change str_status
                }
                else if (cmd->status == 1) {
                    strcpy(cmd->str_status, "EXIT(1)");
                }
                cmd_fetch_output(cmd); // Print output on status change but only first time process finishes 
                printf("@!!! %s[#%d]: EXIT(%d)\n", cmd->name, cmd->pid, cmd->status);
            }
            
        }                                                                         
    }                        
}                          

char *read_all(int fd, int *nread){
    int max_size = BUFSIZE, cur_pos = 0;         // initial max and position
    char *buf = malloc(BUFSIZE+1);             // allocate 1 byte of intial space
    int bytes_read = 1;
    
 do {  
    bytes_read = read(fd, buf+cur_pos, max_size-cur_pos);//-cur_pos    
  
    cur_pos+=bytes_read;                // Keep track of our current position in the buffer 
                                       
    if(cur_pos >= max_size){            // update current input
                            // check if more space is needed
        max_size *= 2;                               // double size of buffer
        char *newbuf = realloc(buf, max_size+1);     // pointer to either new or old location re-allocate, copies characters to new space if needed
        if(!newbuf){                            // check that re-allocation succeeded
            printf("ERROR: reallocation failed\n");    // if not...
            exit(1);                                  
        } 
        buf = newbuf;                              
    } 
    
    } while(bytes_read>0);
    *nread = cur_pos;
    buf[cur_pos] = '\0';       
return buf;   
 

}
void cmd_fetch_output(cmd_t *cmd){
    int bytes_read = 0;
    
    if(cmd->finished == 0){
        printf("%s[%d] not finished yet", (cmd->name), (cmd->pid));
    }
    else {
        if(cmd->output==NULL){
        
        cmd->output = read_all(cmd->out_pipe[PREAD], &bytes_read); // Take output from read end of out_pipe, return buf which is assigned to cmd->output
    
        cmd->output_size = bytes_read;
        
        }
        close(cmd->out_pipe[PREAD]); // Close read end of pipe
    }
   
}


void cmd_print_output(cmd_t *cmd){ 
    
    if(cmd->output == NULL) {
        printf("%s[#%d] : output not ready\n", (cmd->name), (cmd->pid));
    }
    
    else { 
        write(STDOUT_FILENO, cmd->output, BUFSIZE); // Restore output to STDOUT
        printf("%p\n",(cmd->output));

    }

}

