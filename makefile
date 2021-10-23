all: client server

client:
	gcc -o client.out client_main.c landenthornton.c michael_somdecerff.c -pthread -std=c11

server:
	gcc -o server.out RODEN_Serv.c landenthornton.c michael_somdecerff.c -pthread -std=c11

clean:
	rm *.out
	rm 2021*.txt