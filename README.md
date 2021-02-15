# IPC-in-Linux
Inter-process communication in Linux

**Design 1: Shared File**
<br>
• There is a shared file called design1.dat. All communications are carried out on this file.
<br>
• Clients can write to the file, server can read and write to the file.
<br>
• We can create as many clients as we want. There is only one server.
<br>
• Chat outputs are visible only on the server’s terminal. Client terminals are used for only inputs.
<br>
• The file is locked and unlocked before and after read/write actions at all times.

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
• There is a shared memory. All communications are carried out on this memory.
<br>
• Clients can write to the memory, server can read and write to the memory.
<br>
• We can create as many clients as we want. There is only one server.
<br>
• Chat outputs are visible only on the server’s terminal. Client terminals are used for only inputs.
<br>
• The memory is locked and unlocked using semaphores before and after read/write actions at all times.
<br>
• There are two semaphores: one for reader(server) one for writing permissions.
<br>
• Server opens the shared memory and waits for a client to write something into the memory (waits on a semaphore (read_semaphore which is initially 0))
<br>

To run the Design 2:
1) Run the client:
```
gcc design_2/client.c -o client -lrt -pthread
./client
```
2) Enter client name
3) Enter the message to be written on shared memory
4) Run the server to see messages:
```
gcc design_2/server.c -o server -lrt -pthread
./server
```

**Design 3: Unnamed Pipes**
<br>
• There is an unnamed pipeline. All communications are carried out on this pipeline.
<br>
• Clients can write to the pipeline, server can read and write to the pipeline.
<br>
• Chat outputs are visible only on the server’s terminal. Client terminals are used for only inputs.
<br>
• The pipeline is locked and unlocked using semaphores before and after read/write actions at all times.
<br>
• There are two semaphores: one for reader(server) one for writing permissions.
<br>

To run the Design 3:
1) Run the client:
```
gcc design_3/client.c
./a.out
```
2) Enter client name
3) Enter the message to be written on shared pipeline
4) Run the server to see messages:
```
gcc design_3/server.c
./a.out
```

**Design 4: Named Pipes**
<br>
• This is similar to unnamed pipes question. However, this time there is no parent-child relation. 
<br>
• We can create as many clients as we want.
<br>

**Design 5: Message Queues**
<br>
• This is similar to the named pipes.
<br>
• There is a single semaphore, only for the writers.
<br>

**Design 6: Sockets**
<br>
• There is a single socket.
<br>
• There is a single semaphore.
<br>

**Design 7: Sockets + Signals**
<br>
• This is same as the previous design.
<br>
• Server is able to kill the clients by receiving a message “killme”. Server sends a kill signal to the client and the client will be terminated.
<br>
