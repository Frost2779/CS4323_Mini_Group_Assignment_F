-----Compiling-----
To compile this project it must be done on linux, as it is reliant of linux specific syscalls.

---Make---
To compile the project with Make, simply run make with the following command:
make

---Manual---
If for some reason that Make is not working, you can compile the project manually with the following commands:
gcc -o client.out client_main.c landenthornton.c michael_somdecerff.c -pthread -std=c11
gcc -o server.out RODEN_Serv.c landenthornton.c michael_somdecerff.c -pthread -std=c11

-----Running-----
To run this project you will start up both the server and client separately in different terminal windows

---Client---
To start the client, run the following command:
./client.out

---Server---
To start the server, run the following command:
./server.out