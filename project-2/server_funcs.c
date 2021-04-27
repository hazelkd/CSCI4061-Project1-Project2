#include "blather.h"

client_t *server_get_client(server_t *server, int idx){
    //do I need to do something for the size, or is it unknown? 
    if (idx > server->n_clients){
        printf("Index is greater than n_clients, program is exiting\n");
        exit(0);
    }
    else{
        client_t *givenIndex;
        *givenIndex = server->client[idx];
    }
}
// Gets a pointer to the client_t struct at the given index. If the
// index is beyond n_clients, the behavior of the function is
// unspecified and may cause a program crash.

void server_start(server_t *server, char *server_name, int perms){
    log_printf("BEGIN: server_start()\n");              // at beginning of function
    *server->server_name = server_name; // start the server with the given name dont know if this should be a pointer
    remove("server_name.fifo"); //remove any existing file of this name
    mkfifo("server_name.fifo", S_IRUSR | S_IWUSR); //join fifo created
    server->join_fd = open("server_name.fifo", O_RDWR); //open fifo and store fd //not exactly sure if its read and write
    log_printf("END: server_start()\n");                // at end of function
}
// Initializes and starts the server with the given name. A join fifo
// called "server_name.fifo" should be created. Removes any existing
// file of that name prior to creation. Opens the FIFO and stores its
// file descriptor in join_fd.
//
// ADVANCED: create the log file "server_name.log" and write the
// initial empty who_t contents to its beginning. Ensure that the
// log_fd is position for appending to the end of the file. Create the
// POSIX semaphore "/server_name.sem" and initialize it to 1 to
// control access to the who_t portion of the log.
// 
// LOG Messages:
// log_printf("BEGIN: server_start()\n");              // at beginning of function
// log_printf("END: server_start()\n");                // at end of function

void server_shutdown(server_t *server){
    printf("SERVER #%5d: Signalled to shut down\n", getpid());
    close(server->join_fd);
    server->join_ready = 0;
    remove("join_fd.fifo");
    mesg_t msg;
    msg.kind = BL_SHUTDOWN;
    // How to send message?

}


// Shut down the server. Close the join FIFO and unlink (remove) it so
// that no further clients can join. Send a BL_SHUTDOWN message to all
// clients and proceed to remove all clients in any order.
// 
// ADVANCED: Close the log file. Close the log semaphore and unlink
// it.
//
// LOG Messages:
// log_printf("BEGIN: server_shutdown()\n");           // at beginning of function
// log_printf("END: server_shutdown()\n");             // at end of function

int server_add_client(server_t *server, join_t *join){
    if (server->n_clients == MAXCLIENTS){
        return 1;
    }
    int index = -1;                                              // search for name matching request 
    for(int i=0; data[i][0][0] != '\0'; i++){
      if( strcmp(join->name, server->client[i][0])==0 ){
        index = i;                                               // found matching record
        client_t found_client = server->client[i][0];
        break;
      }
    }
    if(index == -1){  
      return 1;                                           // matching name not found
    }
    server->client[server->n_clients] = found_client;
    // Does the string name of the fifo matter?
    // mkfifo in this function?
    mkfifo("msg.fifo", S_IRUSR | S_IWUSR);
    found_client->to_client_fd = open("msg.fifo",O_RDWR);
    found_client->to_server_fd = open("msg.fifo",O_RDWR);
    found_client->data_ready = 0;
    return 0;

}
// Adds a client to the server according to the parameter join which
// should have fileds such as name filed in.  The client data is
// copied into the client[] array and file descriptors are opened for
// its to-server and to-client FIFOs. Initializes the data_ready field
// for the client to 0. Returns 0 on success and non-zero if the
// server as no space for clients (n_clients == MAXCLIENTS).
//
// LOG Messages:
// log_printf("BEGIN: server_add_client()\n");         // at beginning of function
// log_printf("END: server_add_client()\n");           // at end of function

int server_remove_client(server_t *server, int idx){
    close(server->client[idx].to_client_fd);
    close(server->client[idx].to_server_fd);
    remove(server->client[idx].to_client_fname);
    remove(server->client[idx].to_server_fname);

    for (int i = idx; i < server->n_clients - 1; i++) {
        server->client[idx] = server->client[idx+1];
    }
    if (server->n_clients != 0){
        server->n_clients = server->n_clients-1;
        return 0;
    }
    return 1;
    //need to return 0 or 1 - DONE
}
// Remove the given client likely due to its having departed or
// disconnected. Close fifos associated with the client and remove
// them.  Shift the remaining clients to lower indices of the client[]
// preserving their order in the array; decreases n_clients. Returns 0
// on success, 1 on failure.

void server_broadcast(server_t *server, mesg_t *mesg){
    // Loop through server->client
    int num_loops = server->n_clients;
    for(int i=0; num_loops; i++){ // Or num_loops +1 ?
        // open the fifo ?
        write(server->client[i].to_client_fd, mesg, strlen(mesg)); 
        write(server->client[i].to_server_fd, mesg, strlen(mesg));  // ?
    } 

}
// Send the given message to all clients connected to the server by
// writing it to the file descriptors associated with them.
//
// ADVANCED: Log the broadcast message unless it is a PING which
// should not be written to the log.

void server_check_sources(server_t *server){

}
// Checks all sources of data for the server to determine if any are
// ready for reading. Sets the servers join_ready flag and the
// data_ready flags of each if client if data is ready for them.
// Makes use of the poll() system call to efficiently determine which
// sources are ready.
// 
// NOTE: the poll() system call will return -1 if it is interrupted by
// the process receiving a signal. This is expected to initiate server
// shutdown and is handled by returning immediagely from this function.
// 
// LOG Messages:
// log_printf("BEGIN: server_check_sources()\n");             // at beginning of function
// log_printf("poll()'ing to check %d input sources\n",...);  // prior to poll() call
// log_printf("poll() completed with return value %d\n",...); // after poll() call
// log_printf("poll() interrupted by a signal\n");            // if poll interrupted by a signal
// log_printf("join_ready = %d\n",...);                       // whether join queue has data
// log_printf("client %d '%s' data_ready = %d\n",...)         // whether client has data ready
// log_printf("END: server_check_sources()\n");               // at end of function

int server_join_ready(server_t *server){
    return server->join_ready;
}
// Return the join_ready flag from the server which indicates whether
// a call to server_handle_join() is safe.

void server_handle_join(server_t *server){
    int status = server_join_ready(server);
    if (status) { //I think this is how to indicate true? 
        join_t newClient;
        int client_fifo_fd = open(server->join_fd, O_RDWR);
        int nread = read(client_fifo_fd, newClient, 255); 
         
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
    int status = server_client_ready(server, idx);
    if (status){
        
    }
}
// Process a message from the specified client. This function should
// only be called if server_client_ready() returns true. Read a
// message from to_server_fd and analyze the message kind. Departure
// and Message types should be broadcast to all other clients.  Ping
// responses should only change the last_contact_time below. Behavior
// for other message types is not specified. Clear the client's
// data_ready flag so it has value 0.
//
// ADVANCED: Update the last_contact_time of the client to the current
// server time_sec.
//
// LOG Messages:
// log_printf("BEGIN: server_handle_client()\n");           // at beginning of function
// log_printf("client %d '%s' DEPARTED\n",                  // indicates client departed
// log_printf("client %d '%s' MESSAGE '%s'\n",              // indicates client message
// log_printf("END: server_handle_client()\n");             // at end of function 

void server_tick(server_t *server);
// ADVANCED: Increment the time for the server

void server_ping_clients(server_t *server);
// ADVANCED: Ping all clients in the server by broadcasting a ping.

void server_remove_disconnected(server_t *server, int disconnect_secs);
// ADVANCED: Check all clients to see if they have contacted the
// server recently. Any client with a last_contact_time field equal to
// or greater than the parameter disconnect_secs should be
// removed. Broadcast that the client was disconnected to remaining
// clients.  Process clients from lowest to highest and take care of
// loop indexing as clients may be removed during the loop
// necessitating index adjustments.

void server_write_who(server_t *server);
// ADVANCED: Write the current set of clients logged into the server
// to the BEGINNING the log_fd. Ensure that the write is protected by
// locking the semaphore associated with the log file. Since it may
// take some time to complete this operation (acquire semaphore then
// write) it should likely be done in its own thread to preven the
// main server operations from stalling.  For threaded I/O, consider
// using the pwrite() function to write to a specific location in an
// open file descriptor which will not alter the position of log_fd so
// that appends continue to write to the end of the file.

void server_log_message(server_t *server, mesg_t *mesg);
// ADVANCED: Write the given message to the end of log file associated
// with the server.
