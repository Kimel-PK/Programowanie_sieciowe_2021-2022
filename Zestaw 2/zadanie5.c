#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;

static struct sockaddr_in* adres = NULL;

void zwolnijPamiec () {
	free (adres);
}

int main (int argc, char const *argv[]) {
	
	int port = atoi (argv[1]);
	adres = malloc(sizeof(struct sockaddr_in));
	atexit (zwolnijPamiec);
	
    adres->sin_family = AF_INET;
    adres->sin_port = htons(port);
    adres->sin_addr.s_addr = htonl(INADDR_ANY);
	
	int gniazdko = socket(AF_INET, SOCK_STREAM, 0);
	bind (gniazdko, (struct sockaddr*)adres, sizeof(adres));
	listen (gniazdko, 5);
	
	while (1) {
		int klient = accept (gniazdko, NULL, NULL);
		
		write(klient, "Hello, world!\r\n", 15);
		
		close(klient);
	}
	
}