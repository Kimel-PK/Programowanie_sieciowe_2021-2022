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
	
	// pobieranie numeru portu z argumentów linii poleceń
	int port = atoi (argv[1]);
	
	// zamykanie gniazdka w razie błędu lub przerwania Ctrl+C
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
		
		// bufor jest o 1 większy ze względu na sprawdzanie sekwencji \r\n dla netcata
		char bufor[ROZMIAR_DATAGRAMU + 1];
		bufor[ROZMIAR_DATAGRAMU + 1] = '\n';
		unsigned long int suma = 0;
		char odczytanaLiczba[ROZMIAR_DATAGRAMU];
		bool czyNetcat = false;
		bool blad = false;
		
		// oczekiwanie na ramkę
        if (recvfrom(gniazdko, bufor, sizeof(bufor), 0, (struct sockaddr *)&klient, &klientRozmiar) == -1) {
			perror ("Blad recvfrom()");
            exit (EXIT_FAILURE);
		}
		
		// odebrano datagram
		
		for (int i = 0, j = 0; i < ROZMIAR_DATAGRAMU; i++) {
			
			printf ("pozycja %d znak %d\n", i, bufor[i]);
			
			// możliwym znakiem końca liczby jest spacja, \n lub \r\n
			if (bufor[i] == ' ' || bufor[i] == '\n' || (bufor[i] == '\r' && bufor[i + 1] == '\n')) {
				
				// znaleziono spacje lub znak konca linii
				
				odczytanaLiczba[j] = '\0';
				
				unsigned long int przekonwertowanaLiczba = atoi (odczytanaLiczba);
				// sprawdzamy czy odczytana liczba została poprawnie przekonwertowana na typ unsigned long int
				unsigned long int testowanaLiczba = przekonwertowanaLiczba;
				for (int k = j - 1; k >= 0; k--) {
					if (odczytanaLiczba[k] - '0' != testowanaLiczba % 10) {
						blad = true;
						break;
					}
					testowanaLiczba /= 10;
				}
				
				// sprawdzamy czy podczas sumowania wystąpił błąd przepełnienia
				if (suma > suma + przekonwertowanaLiczba || blad == true) {
					blad = true;
					break;
				}
				
				suma += przekonwertowanaLiczba;
				j = 0;
				
				// jeśli zakończeniem linii jest \n to znaczy że odebraliśmy danę od netcata
				if (bufor[i] == '\n') {
					czyNetcat = true;
					break;
				}
				
			} else if (bufor[i] == '\n' || (bufor[i] == '\r' && bufor[i + 1] == '\n')) {
				// trafiliśmy na koniec datagramu wysłanego przez netcata
				czyNetcat = true;
				break;
			} else if (bufor[i] >= '0' && bufor[i] <= '9') {
				// wczytujemy kolejną cyfrę
				printf ("wczytano cyfre %c\n", bufor[i]);
				odczytanaLiczba[j] = bufor[i];
				j++;
				// liczba przekracza ULONG_MAX na poziomie odbioru danych, zwracamy błąd
				if (j == 11) {
					blad = true;
					break;
				}
			} else {
				// wczytano znak ASCII, który nie jest cyfrą
				blad = true;
				break;
			}
		}
		
		if (!blad) {
			
			// konwertujemy sumę liczb na tekst
			int dlugoscOdpowiedzi = 0;
			if (czyNetcat) {
				dlugoscOdpowiedzi = sprintf(odczytanaLiczba, "%ld\n", suma);
			} else {
				dlugoscOdpowiedzi = sprintf(odczytanaLiczba, "%ld", suma);
			}
			
			// odsyłamy odpowiedź
			printf ("obliczona suma %ld, odsylanie: %s, długość datagramu: %d\n", suma, odczytanaLiczba, dlugoscOdpowiedzi);
			if (sendto(gniazdko, odczytanaLiczba, dlugoscOdpowiedzi, 0, (struct sockaddr *)&klient, klientRozmiar) == -1) {
				perror ("Blad sendto()");
				exit (EXIT_FAILURE);
			}
		} else {
			// odsyłamy komunikat o błędzie
			if (sendto(gniazdko, "ERROR\n", 6, 0, (struct sockaddr *)&klient, klientRozmiar) == -1) {
				perror ("Blad sendto()");
				exit (EXIT_FAILURE);
			}
		}
    }
}