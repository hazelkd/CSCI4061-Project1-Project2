#include <pthread.h>
#include <termios.h>
#include "blather.h"

simpio_t simpio_actual;
simpio_t *simpio = &simpio_actual;

client_t client_actual;
client_t *client = &client_actual;

pthread_t user_thread;          // thread managing user input
pthread_t background_thread;

void *user_worker(void *x){

    while(!simpio->end_of_input){
      
      simpio_reset(simpio);
      iprintf(simpio, ""); 
      
      while(!simpio->line_ready && !simpio->end_of_input){          // read until line is complete
        simpio_get_char(simpio); 
      }
      if(simpio->line_ready){

        //iprintf(simpio, "%s You entered: %s\n",client->name,simpio->buf);
        mesg_t newMes = {};
        strncpy(newMes.body, simpio->buf, strlen(simpio->buf));
        strncpy(newMes.name, client->name, strlen(client->name));
        //log_printf("Nnew message body: %s New msg name: %s\n", newMes.body, newMes.name);
        newMes.kind = BL_MESG;
        write(client->to_server_fd, &newMes, sizeof(mesg_t));  
      }
    }
  iprintf(simpio,"End of Input, Departing\n");
  pthread_cancel(background_thread); 
  mesg_t newMes2 = {};              // Making a departed message to send to server to print
  newMes2.kind = BL_DEPARTED;
  strncpy(newMes2.name, client->name, strlen(client->name)); 
  write(client->to_server_fd, &newMes2, sizeof(mesg_t)); 


  return NULL;
}
void *background_worker(void *x){
  
  int status = 1;
  while(status){
    int nread = 0;

    mesg_t msg = {};

    nread = read(client->to_client_fd, &msg, sizeof(mesg_t));    // Server is getting what client wrote to the to_server_fd
  
    if(nread == -1){
      status = 0;
      break;
    }
    
    else if (nread != 0){
      // If no more messages to be read, determine message type for next action
      if(msg.kind == BL_JOINED){
        iprintf(simpio, "-- %s JOINED --\n", msg.name);
      }
      else if(msg.kind == BL_MESG){
        iprintf(simpio, "[%s] : %s\n", msg.name, msg.body);
      }
      else if(msg.kind == BL_DEPARTED){
        iprintf(simpio, "-- %s DEPARTED --\n", msg.name);
      }
      if(msg.kind == BL_SHUTDOWN){
        iprintf(simpio, "!!! server is shutting down !!!\n");
        status = 0;
      }
    }
  }
  pthread_cancel(user_thread); // When no more messages to be read

  return NULL;
}

int main(int argc, char *argv[]){
  

  char prompt[MAXNAME];
  snprintf(prompt, MAXNAME, "%s>> ",argv[2]); // create a prompt string
  simpio_set_prompt(simpio, prompt);         // set the prompt
  simpio_reset(simpio);                      // initialize io
  simpio_noncanonical_terminal_mode();       // set the terminal into a compatible mode

  char server_name[MAXPATH];
  memset(server_name, 0, MAXPATH);
  strncpy(server_name, argv[1], strlen(argv[1]));

  char user_name[MAXPATH];
  memset(user_name, 0, MAXPATH);
  strncpy(user_name, argv[2], strlen(argv[2]));

  char nameClient[MAXPATH*2+13];
  memset(nameClient, 0, MAXPATH*2+13);

  char nameServer[MAXPATH*2+13];
  memset(nameServer, 0, MAXPATH*2+13);

  snprintf(nameClient, MAXPATH*2+13, "%s%s%s", server_name, user_name, "toClient.fifo");
  snprintf(nameServer, MAXPATH*2+13, "%s%s%s", server_name, user_name, "toServer.fifo");
  strncpy(client->to_client_fname, nameClient, strlen(nameClient));
  strncpy(client->to_server_fname, nameServer, strlen(nameServer));

  strncpy(client->name, user_name, strlen(user_name));

  mkfifo(client->to_client_fname, DEFAULT_PERMS);      //join fifo created
  client->to_client_fd = open(client->to_client_fname, O_RDWR);

  mkfifo(client->to_server_fname, DEFAULT_PERMS);
  client->to_server_fd = open(client->to_server_fname, O_RDWR); //specific to server and client

  join_t request = {};
  strncpy(request.name, user_name, strlen(user_name));
  strncpy(request.to_client_fname, client->to_client_fname, strlen(client->to_client_fname));
  strncpy(request.to_server_fname, client->to_server_fname, strlen(client->to_server_fname));

 
  //copy info
  char server_fifo[MAXPATH+5];
  memset(server_fifo, 0, MAXPATH+5);
  snprintf(server_fifo, MAXPATH+5, "%s%s", server_name, ".fifo");
  int serverFD = open(server_fifo, O_RDWR);
  write(serverFD, &request, sizeof(join_t));

  pthread_create(&user_thread,NULL, user_worker, NULL);     // start user thread to read input
  pthread_create(&background_thread, NULL, background_worker, NULL); // start thread to listen to info for server 

  pthread_join(user_thread, NULL);
  pthread_join(background_thread, NULL);
  
  simpio_reset_terminal_mode(); // restore terminal output
  printf("\n");                 // newline just to make returning to the terminal prettier
  return 0;
}

