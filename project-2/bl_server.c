
#include "blather.h"
#include <pthread.h>
#include <termios.h>
#include <string.h>

//The server main() function in its simplest form boils down to the following pseudocode.

/*
REPEAT:
  check all sources
  handle a join request if one is ready
  for each client{
    if the client is ready handle data from it
  }
}
*/
int KEEP_GOING = 1;

void sig_handle(int signal){
    KEEP_GOING = 0;
}

int main(int argc, char *argv[]) {

    struct sigaction sig = {};
    sigemptyset(&sig.sa_mask);
    sig.sa_handler = sig_handle;
    sig.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sig,NULL);
    sigaction(SIGTERM, &sig, NULL);

    server_t server = {};

    strncpy(argv[1], server.server_name, strlen(argv[1]));
    char *fifo_name = ".fifo";
    char *name_server = server.server_name;
    strcat(name_server, fifo_name);

    mkfifo(name_server, S_IRUSR | S_IWUSR);

    server_start(&server, name_server, DEFAULT_PERMS);

    while(KEEP_GOING){
        server_check_sources(&server);
        if(server_join_ready(&server)){
            server_handle_join(&server);
        }
        for (int i = 0; i < server.n_clients; i++) {
            if(server_client_ready(&server, i)){
                server_handle_client(&server, i);
            }
            else {
                break;
            }
        }
    }
    return 0;
}