#include <pthread.h>
#include <termios.h>
#include "blather.h"
//#include "simpio.c"

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

void *user_worker(void *x){
    
    //char buf[1024]; int nread;
    
    while(!simpio->end_of_input){
      simpio_reset(simpio);
      iprintf(simpio, ""); 
      
      while(!simpio->line_ready && !simpio->end_of_input){          // read until line is complete
        simpio_get_char(simpio);
        iprintf(simpio, "%2d You entered: %s\n",client->name,simpio->buf); // Should we print this ?
      }
      if(simpio->line_ready){
        mesg_t *newMes = NULL;
        strcpy(simpio->buf, newMes->body);
        strcpy(client->name, newMes->name);
        newMes->kind = BL_MESG;
        write(client->to_server_fd, newMes, strlen(newMes->body));  
      }
    }
  iprintf(simpio,"End of input, departing.\n");

  mesg_t *newMes2 = NULL;
  newMes2->kind = BL_DEPARTED;
  strcpy(client->name, newMes2->name);

  write(client->to_server_fd, newMes2->body, strlen(newMes2->body)); 

  pthread_cancel(background_thread); 

  return NULL;
}
void *background_worker(void *x){
  //char buf[1024];
  
  while(1){
    int nread = 0;

    mesg_t *msg = NULL;

    nread = read(client->to_client_fd, msg, 1024);              // Can we read a whole message?

    if(nread == 0){
      break;
    }

    if(msg->kind == BL_SHUTDOWN){
      pthread_cancel(user_thread);
    }
    //buf[nread] = '\0';
    iprintf(simpio, "%s Message: |%s|\n", client->name, msg->body);
  }
  return NULL;
}

int main2(int argc, char *argv[]){
  

  char prompt[MAXNAME];
  snprintf(prompt, MAXNAME, "%s>> ","fgnd"); // create a prompt string
  simpio_set_prompt(simpio, prompt);         // set the prompt
  simpio_reset(simpio);                      // initialize io
  simpio_noncanonical_terminal_mode();       // set the terminal into a compatible mode

  //char buf[1024]; int nread;
  server_t *server = NULL;
  while(!simpio->line_ready && !simpio->end_of_input){          // read until line is complete - server
    simpio_get_char(simpio);
    
   
    if(simpio->line_ready){
      strcpy(simpio->buf, server->server_name);
      server_start(server, server->server_name, DEFAULT_PERMS); 
    }
  }
  
  while(!simpio->line_ready && !simpio->end_of_input){          // read until line is complete - client 
        simpio_get_char(simpio);
    
    if(simpio->line_ready){
      
      client_t *client = NULL;
      strcpy(simpio->buf, client->name); // I think we'll have to use the same buffer cause simpio doesn't have 2

    }
  }
  mkfifo("client_name.fifo", S_IRUSR | S_IWUSR);      //join fifo created
  client->to_client_fd = open("client_name.fifo", O_RDWR);
  strcpy("client_name.fifo",client->to_client_fname);

  mkfifo("server_name.fifo", S_IRUSR | S_IWUSR);
  client->to_server_fd = open("server_name.fifo", O_RDWR);
  strcpy("server_name.fifo",client->to_server_fname);

  server_handle_join(server); //this doesn't recognize the server that we opened above

  pthread_create(&user_thread,   NULL, user_worker,   NULL);     // start user thread to read input
  pthread_create(&background_thread, NULL, background_worker, NULL); // start thread to listen to info for server 

  pthread_join(user_thread, NULL);
  pthread_join(background_thread, NULL);
  
  simpio_reset_terminal_mode(); // restore terminal output
  printf("\n");                 // newline just to make returning to the terminal prettier
  return 0;
}

