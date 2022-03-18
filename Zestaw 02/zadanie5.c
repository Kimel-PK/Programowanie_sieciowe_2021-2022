#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

int gniazdko = -1;
struct sockaddr_in* adres;

void zwolnijPamiec () {
	if (gniazdko != -1)
		close (gniazdko);
	
	free (adres);
}

int main (int argc, char const *argv[]) {
	
	int port = atoi (argv[1]);
	
	adres = malloc(sizeof(struct sockaddr_in));
	
	if (adres == NULL) {
		perror ("Blad malloc()");
		exit (EXIT_FAILURE);
	}
	
	atexit (zwolnijPamiec);
	
    adres->sin_family = AF_INET;
    adres->sin_port = htons(port);
    adres->sin_addr.s_addr = htonl(INADDR_ANY);
	
	gniazdko = socket(AF_INET, SOCK_STREAM, 0);
	if (gniazdko == -1) {
		perror ("Blad socket()");
		exit (EXIT_FAILURE);
	}
	
	if (bind (gniazdko, (struct sockaddr*)adres, sizeof(struct sockaddr_in)) == -1) {
		perror ("Blad bind()");
		exit (EXIT_FAILURE);
	}
	
	if (listen (gniazdko, 5) == -1) {
		perror ("Blad listen()");
		exit (EXIT_FAILURE);
	}
	
	while (1) {
		int klient = accept (gniazdko, NULL, NULL);
		
		write(klient, "Hello, world!\r\n", 15);
		
		if (close(klient) == -1) {
			perror ("Blad close()");
			exit (EXIT_FAILURE);
		}
	}
	
}