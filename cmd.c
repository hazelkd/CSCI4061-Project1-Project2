#include <stdio.h>
#include <stdlib.h> //don't know if we need this
#include "commando.h"

cmd_t *cmd_new(char *argv[]) {
    cmd_t *newCmd = (cmd_t*)malloc(sizeof(*argv));
    
    char *target[sizeof(*argv)+1];
    for (int i = 0; i < sizeof(*argv); i++) {
        target[i] = strdup(argv[i]);
    }
    target[sizeof(*argv) + 1] = NULL;

    //dont exactly know what to do if the end is not NULL, don't know what happens
    if (newCmd->argv[sizeof(newCmd->argv)+1] != NULL) {
        printf("we have a problem! No NULL");
    }

    *newCmd->name = *argv[0]; //think this line is wrong
    newCmd->finished = 0;
    snprintf(newCmd->str_status, 5, "INIT");
    newCmd->status = -1;
    newCmd->output = NULL;
    newCmd->output_size = -1;
    newCmd->pid = -1; //0 I think?
    *newCmd->out_pipe = -1; //don't know why this only works with *

// Allocates a new cmd_t with the given argv[] array. Makes string
// copies of each of the strings contained within argv[] using
// strdup() as they likely come from a source that will be
// altered. Ensures that cmd->argv[] is ended with NULL. Sets the name
// field to be the argv[0]. Sets finished to 0 (not finished yet). Set
// str_status to be "INIT" using snprintf(). Initializes the remaining
// fields to obvious default values such as -1s, and NULLs.
}

void cmd_free(cmd_t *cmd){
    //don't know if deallocates a cmd structure is description or task    
    for(int i = 0; i < sizeof(cmd->argv); i++) {
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

void cmd_start(cmd_t *cmd);{

char *child_argv = { //? ,NULL};
pid_t child_pid = fork();
if(pid == 0){
  execvp(cmd, child_argv); 
  //use dup2() command 
  //read()
}
else{
   cmd->pid = child_pid; 
   //write()
}
snprintf(cmd->str_status, 5, "RUN");   //?

// Forks a process and executes command in cmd in the process.
// Changes the str_status field to "RUN" using snprintf().  Creates a
// pipe for out_pipe to capture standard output.  In the parent
// process, ensures that the pid field is set to the child PID. In the
// child process, directs standard output to the pipe using the dup2()
// command. For both parent and child, ensures that unused file
// descriptors for the pipe are closed (write in the parent, read in
// the child).
}
void cmd_update_state(cmd_t *cmd, int block);{

if(finished){           // If the finished flag is 1, does nothing.
    return 0;
}
if(block == DOBLOCK){
    int returned_pid = waitpid(pid, &status, 0);
}
else {
    int returned_pid = waitpid(pid, &status, WNOHANG);
}

if(WIFEXITED(status)){          //Uses the macro WIFEXITED to check the returned status for
    finished = 1;                      // whether the command has exited. 
    cmd->status = WEXITSTATUS(status);   //If command has exited, sets the finished field to 1
    printf("@!!! %s %d EXIT(%s)\n", cmd->name, cmd->pid, cmd->str_status); //?
}
cmd_fetch_output(cmd); //?
                               
                                // and sets the cmd->status field to the exit status of the cmd using
 else{                          // the WEXITSTATUS macro. Calls cmd_fetch_output() to fill up the
                                // output buffer for later printing.
     waitpid(pid, &status, block);              //Otherwise, updates the
                                // state of cmd.  Uses waitpid() and the pid field of command to wait
                                // selectively for the given process. Passes block (one of DOBLOCK or
                                // NOBLOCK) to waitpid() to cause either non-blocking or blocking
                                // waits.  

 }                            
// When a command finishes (the first time), prints a status update
// message of the form
//
// @!!! ls[#17331]: EXIT(0)
//
// which includes the command name, PID, and exit status.
}
char *read_all(int fd, int *nread);{
    
  int max_size = 1, cur_pos = 0;                   // initial max and position
  char *buf = malloc(max_size*sizeof(char));   //?? OR  char buf[BUFSIZE];
  
  while(1){      
                  // break if end of input is reached
        nread = read(fd, buf, cur_pos-1)   //???      //number of bytes read       When no data is left in fd,
        if(nread == 0){
            break;
        } 
        buf[nread] = '\0';                                  // sets the integer pointed to by nread to the number of bytes read
  }                                            // and return a pointer to the allocated buffer.                                             
        cur_pos++;                                     // update current input
        if(cur_pos == max_size){                       // check if more space is needed
            max_size *= 2;                               // double size of buffer
            char *new_buf = realloc(buf, max_size*sizeof(char));                              // pointer to either new or old location      // re-allocate, copies characters to new space if needed
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
void cmd_fetch_output(cmd_t *cmd);
// If cmd->finished is zero, prints an error message with the format
// 
// ls[#12341] not finished yet
// 
// Otherwise retrieves output from the cmd->out_pipe and fills
// cmd->output setting cmd->output_size to number of bytes in
// output. Makes use of read_all() to efficiently capture
// output. Closes the pipe associated with the command after reading
// all input.

void cmd_print_output(cmd_t *cmd);
// Prints the output of the cmd contained in the output field if it is
// non-null. Prints the error message
// 
// ls[#17251] : output not ready
//
// if output is NULL. The message includes the command name and PID.
