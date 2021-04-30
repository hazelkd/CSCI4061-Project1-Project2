#include "blather.h"
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
int main() {
    server_t *server;
    // while () ??
    server_check_sources(server);
    if(server_join_ready(server)){
        server_handle_join(server);
    }
    for (int i = 0; i < server->n_clients; i++) {
        if(server_client_ready(server, i)){
            server_handle_client(server, i);
        }
        else {
            //break;
            //might need break here
        }
    }
}