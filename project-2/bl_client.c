/*
To summarize, bl_client will roughly take the following steps.

read name of server and name of user from command line args
create to-server and to-client FIFOs
write a join_t request to the server FIFO
start a user thread to read inpu
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