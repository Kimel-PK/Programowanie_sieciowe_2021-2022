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

int main () {
	
	// zamykanie gniazdka w razie błędu lub przerwania Ctrl+C
	atexit (zamknijPolaczenie);
	
	struct sockaddr_in adres;
    adres.sin_family = AF_INET;
    adres.sin_port = htons(2020);
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
		char odczytanaLiczba[22];
		bool czyNetcat = false;
		bool czyPusty = true;
		bool blad = false;
		
		int datagram = 0;
		
		// oczekiwanie na ramkę
        if ((datagram = recvfrom(gniazdko, bufor, sizeof(bufor), 0, (struct sockaddr *)&klient, &klientRozmiar)) == -1) {
			perror ("Blad recvfrom()");
            exit (EXIT_FAILURE);
		}
		
		// odebrano datagram
		
		int i = 0;
		
		for (int j = 0; i <= datagram; i++) {
			
			// możliwym znakiem końca liczby jest spacja, \n lub \r\n
			if (bufor[i] == ' ' || bufor[i] == '\n' || (bufor[i] == '\r' && bufor[i + 1] == '\n') || i == datagram) {
				
				// znaleziono spacje lub znak konca linii
				
				odczytanaLiczba[j] = '\0';
				
				unsigned long int przekonwertowanaLiczba = strtoul (odczytanaLiczba, '\0', 10);
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
				if (j > 0)
					czyPusty = false;
				j = 0;
				
				// jeśli zakończeniem linii jest \n to znaczy że odebraliśmy danę od netcata
				if (bufor[i] == '\n' || (bufor[i] == '\r' && bufor [i+1] == '\n')) {
					czyNetcat = true;
					break;
				}
				
				// wymuszenie ostatniego podliczenia sumy
				if (i == datagram)
					break;
				
			} else if (bufor[i] == '\n' || (bufor[i] == '\r' && bufor[i + 1] == '\n')) {
				// trafiliśmy na koniec datagramu wysłanego przez netcata
				czyNetcat = true;
				break;
			} else if (bufor[i] >= '0' && bufor[i] <= '9') {
				// wczytujemy kolejną cyfrę
				odczytanaLiczba[j] = bufor[i];
				j++;
				// liczba przekracza ULONG_MAX na poziomie odbioru danych, zwracamy błąd
				if (j == 21) {
					blad = true;
					break;
				}
			} else {
				// wczytano znak ASCII, który nie jest cyfrą
				blad = true;
				break;
			}
		}
		
		if (!blad && i > 0 && !czyPusty) {
			
			// konwertujemy sumę liczb na tekst
			int dlugoscOdpowiedzi = 0;
			if (czyNetcat) {
				dlugoscOdpowiedzi = sprintf(odczytanaLiczba, "%lu\r\n", suma);
			} else {
				dlugoscOdpowiedzi = sprintf(odczytanaLiczba, "%lu", suma);
			}
			
			// odsyłamy odpowiedź
			if (sendto(gniazdko, odczytanaLiczba, dlugoscOdpowiedzi, 0, (struct sockaddr *)&klient, klientRozmiar) == -1) {
				perror ("Blad sendto()");
				exit (EXIT_FAILURE);
			}
		} else {
			// odsyłamy komunikat o błędzie
			if (czyNetcat) {
				if (sendto(gniazdko, "ERROR\r\n", 7, 0, (struct sockaddr *)&klient, klientRozmiar) == -1) {
					perror ("Blad sendto()");
					exit (EXIT_FAILURE);
				}
			} else {
				if (sendto(gniazdko, "ERROR", 5, 0, (struct sockaddr *)&klient, klientRozmiar) == -1) {
					perror ("Blad sendto()");
					exit (EXIT_FAILURE);
				}
			}
		}
    }
}