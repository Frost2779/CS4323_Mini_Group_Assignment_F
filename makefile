all: client server testing

client:
	gcc -o client.out client_main.c landenthornton.c michael_somdecerff.c pipe.c -pthread -std=c11

server:
	gcc -o server.out RODEN_Serv.c landenthornton.c michael_somdecerff.c pipe.c -pthread -std=c11

testing:
	gcc -o testing.out testing_main.c landenthornton.c michael_somdecerff.c pipe.c -pthread -std=c11

clean:
	rm *.out