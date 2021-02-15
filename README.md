# IPC-in-Linux
Inter-process communication in Linux

**Design 1: Shared File**
• There is a shared file called as design1.dat. All communications is carried out on this file.
• Clients can write to the file, server can read and write to the file.
• We can create as many client as we want. There is only one server.
• Chat outputs will only be visible on the server’s terminal. Client terminals is used for only inputs.
• The file is locked and unlocked before and after read/write actions at all times!

To run the Design 1:
1) Run the client:
'''
gcc design_1/client.c
./a.out
'''
2) Enter client name
3) Enter the message to be written on shared file
4) Run the server to see messages:
'''
gcc design_1/server.c
./a.out
'''

**Design 2: Shared Memory**
**Design 3: Unnamed Pipes**
**Design 4: Named Pipes**
**Design 5: Message Queues**
**Design 6: Sockets**
**Design 7: Sockets + Signals**
