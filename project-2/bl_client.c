#include <pthread.h>
#include "blather.h"

simpio_t simpio_actual;
simpio_t *simpio = &simpio_actual;

client_t client_actual;
client_t *client = &client_actual;
pthread_t user_thread;          // thread managing user input
pthread_t background_thread;

/*
To summarize, bl_client will roughly take the following steps.

read name of server and name of user from command line args
create to-server and to-client FIFOs
write a join_t request to the server FIFO
start a user thread to read input
start a server thread to listen to the server
wait for threads to return
restore standard terminal output

user thread{
  repeat:
    read input using simpio
    when a line is ready
    create a mesg_t with the line and write it to the to-server FIFO
  until end of input
  print "End of Input, Departing"
  write a DEPARTED mesg_t into to-server
  cancel the server thread

server thread{
  repeat:
    read a mesg_t from to-client FIFO
    print appropriate response to terminal with simpio
  until a SHUTDOWN mesg_t is read
  cancel the user thread
  */

  //directly strcopy to message and then write that msg to the fifo

void *threadA_func(void *x){
    
    
    char buf[1024]; int nread;
    while(!simpio->line_ready && !simpio->end_of_input){          // read until line is complete
        simpio_get_char(simpio);
    }
    if(simpio->line_ready){
      mesg_t newMes;
      strcopy(simpio->buf, newMes->body);
      write(,)
    }
  printf("Child A closed pipe\n");
  return NULL;
}
//void *threadB_func(void )

int main(int argc, char *argv[]){
  
  
  //printf("Child A closed pipe\n");

  char prompt[MAXNAME];
  snprintf(prompt, MAXNAME, "%s>> ","fgnd"); // create a prompt string
  simpio_set_prompt(simpio, prompt);         // set the prompt
  simpio_reset(simpio);                      // initialize io
  simpio_noncanonical_terminal_mode();       // set the terminal into a compatible mode

  char buf[1024]; int nread;
  while(!simpio->line_ready && !simpio->end_of_input){          // read until line is complete
        simpio_get_char(simpio);
    
    if(simpio->line_ready){
      server_t server;
      strcopy(simpio->buf, server->server_name);
      
      client_t client;
      strcopy(simpio->buf, client->name);
    }
  }

  pthread_create(&user_thread,   NULL, user_worker,   NULL);     // start user thread to read input
  pthread_create(&background_thread, NULL, background_worker, NULL);

  pthread_join(user_thread, NULL);
  pthread_join(background_thread, NULL);
  
  simpio_reset_terminal_mode();
  printf("\n");                 // newline just to make returning to the terminal prettier
  return 0;
}

