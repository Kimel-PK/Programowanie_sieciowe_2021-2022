#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int gniazdko = -1;

void zamknijPolaczenie () {
	if (gniazdko != -1)
		close (gniazdko);
}

int main (int argc, char const *argv[]) {
	
	int port = atoi (argv[1]);
	
	atexit (zamknijPolaczenie);
	
	struct sockaddr_in adres;
    adres.sin_family = AF_INET;
    adres.sin_port = htons(port);
    adres.sin_addr.s_addr = htonl(INADDR_ANY);
	
	gniazdko = socket(AF_INET, SOCK_DGRAM, 0);
	if (gniazdko == -1) {
		perror ("Blad socket()");
		exit (EXIT_FAILURE);
	}
	
	if (bind (gniazdko, (struct sockaddr*)&adres, sizeof(struct sockaddr_in)) == -1) {
		perror ("Blad bind()");
		exit (EXIT_FAILURE);
	}
	
    while (1) {
		
		struct sockaddr_in klient;
        socklen_t klientRozmiar = sizeof(klient);
		
        if (recvfrom(gniazdko, NULL, 0, 0, (struct sockaddr *)&klient, &klientRozmiar) == -1) {
			perror ("Blad recvfrom()");
            exit (EXIT_FAILURE);
		}
		
        if (sendto(gniazdko, "Hello World\n", 13, 0, (struct sockaddr *)&klient, klientRozmiar) == -1) {
			perror ("Blad sendto()");
			exit (EXIT_FAILURE);
		}
    }
}