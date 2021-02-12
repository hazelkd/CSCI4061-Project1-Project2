#include <stdio.h>
#include <stdlib.h> //don't know if we need this
#include "commando.h"

cmd_t *cmd_new(char *argv[]) {
    cmd_t *cmd;
    *cmd->argv = malloc(sizeof(*argv)+1);

     for(int i=0;i++;sizeof(*cmd->argv)){
        *cmd->argv= malloc(strlen(argv[i])); //might need extra byte for 0 byte
        printf("%0x20\n",*cmd);
        //make cmd an array or increment
     }
     //Use str dup & check if includes 0 byte
    //not sure if this is right
    *cmd->argv = strdup(*argv);

    cmd->argv[sizeof(*argv)] = NULL;

    *cmd->name = *cmd->argv[0]; //think this line is wrong
    cmd->finished = 0;
    snprintf(cmd->str_status, 5, "INIT");
    cmd->status = -1;
    cmd->output = NULL;
    cmd->output_size = -1;
    cmd->pid = 0; //I think?
    *cmd->out_pipe = -1; //don't know why this only works with *

// Allocates a new cmd_t with the given argv[] array. Makes string
// copies of each of the strings contained within argv[] using
// strdup() as they likely come from a source that will be
// altered. Ensures that cmd->argv[] is ended with NULL. Sets the name
// field to be the argv[0]. Sets finished to 0 (not finished yet). Set
// str_status to be "INIT" using snprintf(). Initializes the remaining
// fields to obvious default values such as -1s, and NULLs.
}

void cmd_free(cmd_t *cmd){
    for(int i = 0; i < (sizeof(cmd->argv)-1); i++) {
        free(cmd->argv[i]);
    }

    if (cmd->output != NULL) {
        free(cmd->output);
    }

    free(cmd);
// Deallocates a cmd structure. Deallocates the strings in the argv[]
// array. Also deallocats the output buffer if it is not
// NULL. Finally, deallocates cmd itself.
}

void cmd_start(cmd_t *cmd){

char *child_argv = {,NULL};
pid_t pid = fork();
if(pid == 0){
  execvp(cmd, child_argv);  
}

// Forks a process and executes command in cmd in the process.
// Changes the str_status field to "RUN" using snprintf().  Creates a
// pipe for out_pipe to capture standard output.  In the parent
// process, ensures that the pid field is set to the child PID. In the
// child process, directs standard output to the pipe using the dup2()
// command. For both parent and child, ensures that unused file
// descriptors for the pipe are closed (write in the parent, read in
// the child).
}
void cmd_update_state(cmd_t *cmd, int block){

if(cmd->finished){           // If the finished flag is 1, does nothing.
    return 0; //don't think we can return with a void? 
}
if(block == DOBLOCK){
    int returned_pid = waitpid(cmd->pid, &status, 0);
}
else {
    int returned_pid = waitpid(cmd->pid, &status, WNOHANG);
}

if(WIFEXITED(cmd->status)){          //Uses the macro WIFEXITED to check the returned status for
    cmd->finished = 1; 
                         // whether the command has exited. 
    cmd->status = WEXITSTATUS(&status);
    printf("\n", cmd->status);
}                               //If command, sets the finished field to 1
                                // and sets the cmd->status field to the exit status of the cmd using
 else{                          // the WEXITSTATUS macro. Calls cmd_fetch_output() to fill up the
                                // output buffer for later printing.
     waitpid(cmd->pid, &status, block);              //Otherwise, updates the
                                // state of cmd.  Uses waitpid() and the pid field of command to wait
                                // selectively for the given process. Passes block (one of DOBLOCK or
                                // NOBLOCK) to waitpid() to cause either non-blocking or blocking
                                // waits.  

 } 
}                           
// When a command finishes (the first time), prints a status update
// message of the form
//
// @!!! ls[#17331]: EXIT(0)
//
// which includes the command name, PID, and exit status.

char *read_all(int fd, int *nread){
    int max_size = 1, cur_pos = 0;                   // initial max and position
  char *buf = malloc(max_size*sizeof(char));       // allocate 1 byte of intial space
  
  while(1){ 
    int ret = fscanf(fd,"%c", &buf[cur_pos]);    //???      
    if(ret == EOF || ret =='\0'){               // break if end of input is reached
        *nread =                    //number of bytes read       When no data is left in fd,
                                            // sets the integer pointed to by nread to the number of bytes read
                                            // and return a pointer to the allocated buffer.  
        return *buf;                                  
    }          
    cur_pos++;                                     // update current input
    if(cur_pos == max_size){                       // check if more space is needed
      max_size *= 2;                               // double size of buffer
      char *new_buf =                              // pointer to either new or old location
        realloc(buf, max_size*sizeof(char));       // re-allocate, copies characters to new space if needed
      if(new_buf == NULL){                         // check that re-allocation succeeded
        printf("ERROR: reallocation failed\n");    // if not...
        free(buf);                                 // de-allocate current buffer
        exit(1);                                   // bail out
      }
      buf = new_buf;                               // assigns either existin or new location of expanded space
    } 
    free(buf);
    return 0;
}
// Reads all input from the open file descriptor fd. Assumes
// character/text output and null-terminates the character output with
// a '\0' character allowing for printf() to print it later. Stores
// the results in a dynamically allocated buffer which may need to
// grow as more data is read.  Uses an efficient growth scheme such as
// doubling the size of the buffer when additional space is
// needed. Uses realloc() for resizing.   Ensures the return
// string is null-terminated. Does not call close() on the fd as this
// is done elsewhere.
}
void cmd_fetch_output(cmd_t *cmd){
    if(cmd->finished == 0){
        printf("%s[%d] not finished yet", (cmd->name), (cmd->pid));
    }
    else {
        cmd->output_size = sizeof(cmd->output);
        //don't think this is correct
        //Has to be finished with read all and pipes once we learn it
        cmd->output = cmd->out_pipe;
    }
}
// If cmd->finished is zero, prints an error message with the format
// 
// ls[#12341] not finished yet
// 
// Otherwise retrieves output from the cmd->out_pipe and fills
// cmd->output setting cmd->output_size to number of bytes in
// output. Makes use of read_all() to efficiently capture
// output. Closes the pipe associated with the command after reading
// all input.

void cmd_print_output(cmd_t *cmd){
    if(cmd->output == NULL) {
        printf("%s[%d] : output not ready\n", (cmd->name), (cmd->pid));
    }
    //this doesn't work, not quite sure how to print void output might be 
    //with pipes
    else {
       printf("%s\n",(*cmd->output));
    }
}
// Prints the output of the cmd contained in the output field if it is
// non-null. Prints the error message
// 
// ls[#17251] : output not ready
//
// if output is NULL. The message includes the command name and PID.
