#include "blather.h"

client_t *server_get_client(server_t *server, int idx){
   // Gets pointer to client at the given index in server array of clients
    if (idx < server->n_clients){
        return &server->client[idx];
    }
    else {
        printf("Index is greater than n_clients, program is exiting\n");
        exit(0);
    }

    return NULL;
}

void server_start(server_t *server, char *server_name, int perms){
    log_printf("BEGIN: server_start()\n");              // at beginning of function
    
    strncpy(server->server_name, server_name, strlen(server_name));
    char serverName1[MAXPATH];
    memset(serverName1, 0, MAXPATH);
    snprintf(serverName1, MAXPATH+5, "%s%s", server_name, ".fifo");
   
    unlink(serverName1);                        //remove any existing file of this name
    mkfifo(serverName1, DEFAULT_PERMS);         //join fifo "server_name.fifo" created
    server->join_fd = open(serverName1, O_RDWR);    //open fifo and store fd 
    server->join_ready = 0;
    log_printf("END: server_start()\n");                // at end of function
}


void server_shutdown(server_t *server){

    log_printf("BEGIN: server_shutdown()\n");           

    close(server->join_fd); // Close the join FIFO and unlink (remove) it so that no further clients can join.
    
    unlink(server->server_name);
    mesg_t msg = {};
    msg.kind = BL_SHUTDOWN;
    server_broadcast(server, &msg); // Send a BL_SHUTDOWN message to all clients 
    for (int i = 0; i < server->n_clients; i++){
        
        server_remove_client(server, i);   // Remove all clients
    }
   
    log_printf("END: server_shutdown()\n");          
    
}

int server_add_client(server_t *server, join_t *join){

    log_printf("BEGIN: server_add_client()\n"); 

    if (server->n_clients == MAXCLIENTS){ // Returns non-zero if the server as no space for clients
        return 1;
    }                                              // found matching record
    client_t added_client = {};
    // The client data is copied into the client[] array
    strncpy(added_client.name ,join->name, sizeof(join->name));
    strncpy(added_client.to_client_fname,join->to_client_fname, sizeof(join->to_client_fname));
    strncpy(added_client.to_server_fname,join->to_server_fname, sizeof(join->to_server_fname));
    //opening and creating fd to server
    added_client.to_server_fd = open(added_client.to_server_fname, O_RDWR);
    //opening and creating fd to client 
    added_client.to_client_fd = open(added_client.to_client_fname, O_RDWR);
    //intializing data ready to zero
    added_client.data_ready = 0;

    //log_printf("to server fname %s, to client fname %s\n", added_client.to_server_fname, added_client.to_client_fname);
    server->client[server->n_clients] = added_client;
    server->n_clients = server->n_clients + 1;

    log_printf("END: server_add_client()\n");           // at end of function

    return 0; // Returns 0 on success
}

int server_remove_client(server_t *server, int idx){
    
    // Close fifos associated with the client and remove them.
    close(server_get_client(server, idx)->to_client_fd);
    close(server_get_client(server, idx)->to_server_fd);
    // Remove the given client (likely departed or disconnected)
    unlink(server_get_client(server, idx)->to_client_fname);
    unlink(server_get_client(server, idx)->to_server_fname);

    // Shift the remaining clients to lower indices of the client[] 
    // preserving their order in the array; decreases n_clients
    for (int i = idx; i < server->n_clients - 1; i++) {

        server->client[idx] = server->client[idx+1];
    }
    if (server->n_clients != 0){
        server->n_clients = server->n_clients-1;
        return 0; // Returns 0 on success
    }
    return 1; // Returns 1 on fail 
}

void server_broadcast(server_t *server, mesg_t *mesg){
    // Loop through server->client, send given message to all clients connected
    // to the server by writing to the fd associated with them. 
    int num_loops = server->n_clients;
    for(int i=0; i < num_loops; i++){ 
        write(server->client[i].to_client_fd, mesg, sizeof(mesg_t)); 
    } 
}

void server_check_sources(server_t *server){

    log_printf("BEGIN: server_check_sources()\n");
    struct pollfd pfds[server->n_clients+1];             // array of structures for poll, 1 per fd to be monitored

    pfds[0].fd     = server->join_fd;           // populate first entry with server join fd
    pfds[0].events = POLLIN;
    pfds[0].revents = -1;
    
    if (server->n_clients > 0) {
        for(int i = 1; i <= server->n_clients; i++){
            pfds[i].fd     = server->client[i-1].to_server_fd;          // populate other entries with fds
            pfds[i].events = POLLIN;
            pfds[i].revents = -1;        
        }
    }
    log_printf("poll()'ing to check %d input sources\n", 1 + server->n_clients);
    
    int ret = poll(pfds, (server->n_clients + 1), -1); // Determines which sources are ready 
    log_printf("poll() completed with return value %d\n", ret);
    if (ret == -1){
        log_printf("poll() interrupted by a signal\n");
    }
    else{
        if (pfds[0].revents & POLLIN){
            server->join_ready = 1;  
        }   
        log_printf("join_ready = %d\n", server->join_ready);
    
        for(int j = 0; j < server->n_clients; j++){        // If one is ready then set the server and client flags
            if (pfds[j+1].revents & POLLIN) {
                server->client[j].data_ready = 1; 
                log_printf("client %d '%s' data_ready = %d\n", j, server->client[j].name,1);  
            }
            else{
                log_printf("client %d '%s' data_ready = %d\n", j, server->client[j].name, 0);
            }
            
        }
    }
    log_printf("END: server_check_sources()\n");

}

int server_join_ready(server_t *server){
    // Return the join_ready flag from the server which indicates whether
    // a call to server_handle_join() is safe.
    return server->join_ready;
}

void server_handle_join(server_t *server){
    // Called only if server_join_ready() returns true.
    log_printf("BEGIN: server_handle_join()\n");       
    int status = server_join_ready(server);
    if (status) {  
        join_t newRequest = {};
        // Read a join request and add the new client to the server.
        int nread = read(server->join_fd, &newRequest, sizeof(join_t));
            if (nread == 0) {
                log_printf("No request read."); 
            }
            else {
                log_printf("join request for new client '%s'\n",newRequest.name);      // reports name of new client

                server_add_client(server, &newRequest);
                mesg_t msg ={};
                msg.kind = BL_JOINED;
                strncpy(msg.name, newRequest.name, strlen(newRequest.name));
                server_broadcast(server, &msg);
                
                server->join_ready = 0;

                log_printf("END: server_handle_join()\n");                 // at end of function
            }
    }
}

// Call this function only if server_join_ready() returns true. Read a
// join request and add the new client to the server. After finishing,
// set the servers join_ready flag to 0.
//
// LOG Messages:
// log_printf("BEGIN: server_handle_join()\n");               // at beginnning of function
// log_printf("join request for new client '%s'\n",...);      // reports name of new client
// log_printf("END: server_handle_join()\n");                 // at end of function

int server_client_ready(server_t *server, int idx){
    return (server->client[idx].data_ready);
}
// Return the data_ready field of the given client which indicates
// whether the client has data ready to be read from it.

void server_handle_client(server_t *server, int idx){
    // This function should only be called if server_client_ready() returns true.
    log_printf("BEGIN: server_handle_client()\n");        
    int status = server_client_ready(server, idx);
   
    if (status){
        mesg_t newMessage = {};
    
        int nread = read(server->client[idx].to_server_fd, &newMessage, sizeof(mesg_t));
            if (nread == 0) {
                log_printf("No request read."); 
            }
            else{
                // Read a message from to_server_fd and analyze the message kind.
                mesg_kind_t newMessageType;
                newMessageType = newMessage.kind;

                if(newMessageType == BL_DEPARTED) {
                    server_broadcast(server, &newMessage);
                    log_printf("client %d '%s' DEPARTED\n", idx, server->client[idx].name);
                    server_remove_client(server, idx);
                    // indicates client departed
                } 
                else if(newMessageType == BL_MESG){
                    server_broadcast(server, &newMessage);
                    log_printf("client %d '%s' MESSAGE '%s'\n", idx, server->client[idx].name, newMessage.body);   // indicates client message
                }
                // Clear the client's data_ready flag so it has value 0.
                server->client[idx].data_ready = 0;
                log_printf("END: server_handle_client()\n");         
            }
    }
}

void server_tick(server_t *server);


void server_ping_clients(server_t *server);


void server_remove_disconnected(server_t *server, int disconnect_secs);


void server_write_who(server_t *server);


void server_log_message(server_t *server, mesg_t *mesg);

