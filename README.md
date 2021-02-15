# IPC-in-Linux
Inter-process communication in Linux

**Design 1: Shared File**
<br>
• There is a shared file called as design1.dat. All communications is carried out on this file.
<br>
• Clients can write to the file, server can read and write to the file.
<br>
• We can create as many client as we want. There is only one server.
<br>
• Chat outputs will only be visible on the server’s terminal. Client terminals is used for only inputs.
<br>
• The file is locked and unlocked before and after read/write actions at all times!

To run the Design 1:
1) Run the client:
```
gcc design_1/client.c
./a.out
```
2) Enter client name
3) Enter the message to be written on shared file
4) Run the server to see messages:
```
gcc design_1/server.c
./a.out
```

**Design 2: Shared Memory**
<br>
**Design 3: Unnamed Pipes**
<br>
**Design 4: Named Pipes**
<br>
**Design 5: Message Queues**
<br>
**Design 6: Sockets**
<br>
**Design 7: Sockets + Signals**
