#include "blather.h"
#include <pthread.h>

simpio_t simpio_actual;
simpio_t *simpio = &simpio_actual;

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
void *threadA_func(void *x){
    char buf[1024]; int nread;
    while(!simpio->line_ready && !simpio->end_of_input){          // read until line is complete
        simpio_get_char(simpio);
    }
    if(simpio->line_ready){
      mesg_t newMes;
     // newMes = simpio;
      for(int i =0 ; i < simpio->pos; i++){
        newMsg->body = simpio->buf 
      }
      //directly strcopy to message and then write that msg to the fifo
    }
  printf("Child A closed pipe\n");
  return NULL;
}

