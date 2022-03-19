#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define ROZMIAR_DATAGRAMU 65527

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
	
	printf ("bind zadzialal\n");
	printf ("%ld\n", ULONG_MAX);
	
	char maksimum[] = "4294967295";
	
    while (1) {
		
		struct sockaddr_in klient;
        socklen_t klientRozmiar = sizeof(klient);
		
		char bufor[ROZMIAR_DATAGRAMU + 1];
		bufor[ROZMIAR_DATAGRAMU + 1] = '\n';
		unsigned long int suma = 0;
		char odczytanaLiczba[ROZMIAR_DATAGRAMU];
		bool czyNetcat = false;
		bool blad = false;
		
		printf ("oczekiwanie na ramke\n");
		
        if (recvfrom(gniazdko, bufor, sizeof(bufor), 0, (struct sockaddr *)&klient, &klientRozmiar) == -1) {
			perror ("Blad recvfrom()");
            exit (EXIT_FAILURE);
		}
		
		printf ("odebrano datagram\n");
		
		printf ("%s:koniec\n", bufor);
		
		for (int i = 0, j = 0; i < ROZMIAR_DATAGRAMU; i++) {
			
			printf ("pozycja %d znak %d\n", i, bufor[i]);
			
			if (bufor[i] == ' ' || bufor[i] == '\n' || (bufor[i] == '\r' && bufor[i + 1] == '\n')) {
				
				printf ("znaleziono spacje lub znak konca linii\n");
				
				odczytanaLiczba[j] = '\0';
				
				printf ("Odczytana liczba: %s\n", odczytanaLiczba);
				printf ("atoi: %d\n", atoi (odczytanaLiczba));
				
				unsigned long int przekonwertowanaLiczba = atoi (odczytanaLiczba);
				unsigned long int testowanaLiczba = przekonwertowanaLiczba;
				for (int k = j - 1; k >= 0; k--) {
					printf ("odczytanaLiczba[k] = %c\n", odczytanaLiczba[k]);
					if (odczytanaLiczba[k] - '0' != testowanaLiczba % 10) {
						blad = true;
						break;
					}
					testowanaLiczba /= 10;
				}
				
				if (suma > suma + przekonwertowanaLiczba || blad == true) {
					blad = true;
					break;
				}
				
				suma += przekonwertowanaLiczba;
				j = 0;
				
				if (bufor[i] == '\n') {
					czyNetcat = true;
					break;
				}
				
			} else if (bufor[i] == '\n' || (bufor[i] == '\r' && bufor[i + 1] == '\n')) {
				czyNetcat = true;
				break;
			} else if (bufor[i] >= '0' && bufor[i] <= '9') {
				printf ("wczytano cyfre %c\n", bufor[i]);
				odczytanaLiczba[j] = bufor[i];
				j++;
				if (j == 11) {
					blad = true;
					break;
				}
			} else {
				blad = true;
				break;
			}
		}
		
		if (!blad) {
			
			printf ("konwersja sumy na string\n");
			int dlugoscOdpowiedzi = 0;
			if (czyNetcat) {
				dlugoscOdpowiedzi = sprintf(odczytanaLiczba, "%ld\n", suma);
			} else {
				dlugoscOdpowiedzi = sprintf(odczytanaLiczba, "%ld", suma);
			}
			
			printf ("obliczona suma %ld, odsylanie: %s, długość datagramu: %d\n", suma, odczytanaLiczba, dlugoscOdpowiedzi);
			if (sendto(gniazdko, odczytanaLiczba, dlugoscOdpowiedzi, 0, (struct sockaddr *)&klient, klientRozmiar) == -1) {
				perror ("Blad sendto()");
				exit (EXIT_FAILURE);
			}
		} else {
			if (sendto(gniazdko, "ERROR\n", 6, 0, (struct sockaddr *)&klient, klientRozmiar) == -1) {
				perror ("Blad sendto()");
				exit (EXIT_FAILURE);
			}
		}
    }
}