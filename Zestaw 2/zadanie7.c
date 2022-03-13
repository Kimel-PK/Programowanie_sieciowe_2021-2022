#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int gniazdko = -1;
struct sockaddr_in* adres = NULL;

bool drukowalne (const void* buf, int len) {
	
	const char* _buf = buf;
	
	for (int i = 0; i < len; i++) {
		if (_buf[i] < 32 || _buf[i] > 126)
			return false;
	}
	
	return true;
}

void zwolnijPamiec () {
	if (gniazdko != -1)
		close (gniazdko);
	
	free (adres);
}

int main (int argc, char const *argv[]) {
	
	const char* ipv4 = argv[1];
	int port = atoi (argv[2]);
	
	adres = malloc(sizeof(struct sockaddr_in));
	if (adres == NULL) {
		perror ("Blad malloc()");
		exit (EXIT_FAILURE);
	}
	
	atexit (zwolnijPamiec);
	
    adres->sin_family = AF_INET;
    adres->sin_port = htons(port);
    adres->sin_addr.s_addr = inet_addr(ipv4);
	
	gniazdko = socket(AF_INET, SOCK_STREAM, 0);
	if (gniazdko == -1) {
		perror ("Blad socket()");
		exit (EXIT_FAILURE);
	}
	
	if (connect (gniazdko, (struct sockaddr*)adres, sizeof(struct sockaddr_in)) == -1) {
		perror ("Blad connect()");
		exit (EXIT_FAILURE);
	}
	
	char bufor[15];
	
	if (read(gniazdko, &bufor, 15) == -1) {
		perror ("Blad read()");
		exit(EXIT_FAILURE);
	}
	
	if (drukowalne (bufor, 13)) {
		printf ("%s\n", bufor);
	} else {
		printf ("W buforze znalazly sie niedrukowalne znaki");
		exit (EXIT_FAILURE);
	}
	
	if (close(gniazdko) == -1) {
		perror ("Blad close()");
		exit (EXIT_FAILURE);
	}
	
}