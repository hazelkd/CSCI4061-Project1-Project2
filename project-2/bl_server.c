
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

    server_start(&server, argv[1], DEFAULT_PERMS);

    while(KEEP_GOING){
        server_check_sources(&server);

        if(server_join_ready(&server)){
            server_handle_join(&server);
        }
        for (int i = 0; i < server.n_clients; i++) {
            if(server_client_ready(&server, i)){
                //log_printf("Server client %s at %d\n", server.client[i].name, i);
                server_handle_client(&server, i);
            }
        }
    }
    server_shutdown(&server);
    return 0;
}