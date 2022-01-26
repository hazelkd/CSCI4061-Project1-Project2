# CSCI4061-Project1-Project2
CSCI 4061: Intro to Operating Systems. Semester was made up of two main projects, included here.

The course was made up of two portions, part 1 being in the first part of the semester, and part 2 following.

Part 1:
The main goal of this project is to write a simple, quasi-command line shell called commando. It is not as functional
as a standard shell, but has properties that distinguish it separately, like the ability to recall old output.
Our command line shell uses a variety of system calls together to accomplish its purpose. 
We were responsible for writing a variety of funcitions for the command shell, creating data types, completing
tests, and creating multiple files from scratch.

Part 2:
This project focuses on developing a simple chat server and client called blather. The basic usage is in two parts.
The main idea is that the project is like a chat room, as clients interact with the server and send it information.
The server is able to broadcast client input to all other clients in the chat room. The client chats through
the keyboard on their computer to interact. 
Unlike standard internet chat services, blather will be restricted to a single Unix machine and to users with
permission to read related files. 
Some main tasks of our project were to: 
Multiple communicating processes: clients to servers
Communication through FIFOs
Signal handling for graceful server shutdown
Alarm signals for periodic behavior
Input multiplexing with poll()
Multiple threads in the client to handle typed input versus info from the server
