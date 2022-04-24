#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <sys/time.h>

#define ROZMIAR_BUFORA 1024

using namespace std;

void sumator (int gniazdko, struct sockaddr_in adresKlienta) {
	
	char bufor[ROZMIAR_BUFORA];
	char odczytanaLiczba[22];
	int j = 0;
	unsigned long int suma = 0;
	bool koniecZapytania = false; // prawda tylko wtedy gdy poprzednik znakiem było \r
	bool blad = false;
	bool pusteZapytanie = true;
	
    while (1) {
		
		// pobierz nową ramkę
		
		int odczytano = -1;
		
		if ((odczytano = read(gniazdko, bufor, ROZMIAR_BUFORA)) == -1) {
			perror("Połączenie przerwane");
			break;
		}
		
		if (odczytano == 0)
			break;
		
		// przeanalizuj ramkę
		for (int i = 0; i < odczytano; i++) {
			
			if (bufor[i] == ' ' || bufor[i] == '\n') { // trafiono na terminator liczby
				// przekonwertuj odczytaną liczbę i dodaj do sumy
				
				odczytanaLiczba[j] = '\0';
				
				if (j == 0) {
					// spacja na początku lub dwie spacje obok siebie
					blad = true;
				}
				
				unsigned long int przekonwertowanaLiczba = strtoul (odczytanaLiczba, NULL, 10);
				// sprawdzamy czy odczytana liczba została poprawnie przekonwertowana na typ unsigned long int
				unsigned long int testowanaLiczba = przekonwertowanaLiczba;
				for (int k = j - 1; k >= 0; k--) {
					if (odczytanaLiczba[k] - '0' != (int)(testowanaLiczba % 10)) {
						blad = true;
						break;
					}
					testowanaLiczba /= 10;
				}
				
				// sprawdzamy czy podczas sumowania wystąpił błąd przepełnienia
				if (suma > suma + przekonwertowanaLiczba) {
					blad = true;
				}
				
				suma += przekonwertowanaLiczba;
				
				j = 0;
			}
			
			if (bufor[i] == '\r') { // pierwszy znak końca zapytania
				koniecZapytania = true;
				continue;
				
			} else if (bufor[i] == '\n') { // drugi znak końca zapytania
				
				if (koniecZapytania) {
					// zakończ przetwarzanie, odeślij wynik
					
					int dlugoscOdpowiedzi = 0;
					
					// za bufor do przechowania wyniku posłuży tymczasowo odczytanaLiczba
					
					if (!blad && !pusteZapytanie) {
						// konwertujemy sumę liczb na tekst
						dlugoscOdpowiedzi = sprintf(odczytanaLiczba, "%lu\r\n", suma);
					} else {
						// komunikat o błędzie
						dlugoscOdpowiedzi = sprintf(odczytanaLiczba, "ERROR\r\n");
					}
					
					// odsyłamy odpowiedź
					if (write (gniazdko, odczytanaLiczba, dlugoscOdpowiedzi) == -1) {
						perror ("write");
						exit (EXIT_FAILURE);
					}
					
					suma = 0;
					blad = false;
					pusteZapytanie = true;
					
				} else {
					
					blad = true;
				}
				
				koniecZapytania = false;
				continue;
			}
			
			// odczytaliśmy kolejny znak więc \r nie jest już poprzednie
			koniecZapytania = false;
			
			// jeśli dotychczas wystąpił błąd to nie interesuje nas nic innego po za końcem zapytania
			if (blad || bufor[i] == ' ')
				continue;
			
			if (bufor[i] >= '0' && bufor[i] <= '9') {
				// dopisz cyfrę
				
				odczytanaLiczba[j] = bufor[i];
				if (j++ > 21) { // przepełnienie
					blad = true;
				}
				
				pusteZapytanie = false;
			} else {
				// odczytano znak który nie jest ani cyfrą ani \r\n
				blad = true;
			}
		}
    }
}

int main () {
	
	int port = 2020;
	
	// struktury adresów serwera i klienta
	struct sockaddr_in adres;
    adres.sin_family = AF_INET;
    adres.sin_port = htons(port);
    adres.sin_addr.s_addr = htonl(INADDR_ANY);
	struct sockaddr_in adresKlienta;
	socklen_t adresKlienta_len = sizeof (adresKlienta);
	
	// tworzenie gniazdka
	int gniazdko = -1;
	if ((gniazdko = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror ("Blad socket()");
		exit (EXIT_FAILURE);
	}
	
	if (bind (gniazdko, (struct sockaddr*)&adres, sizeof(adres)) == -1) {
		perror ("Blad bind()");
		exit (EXIT_FAILURE);
	}
	
	// dajemy znać jądru że nie będziemy prosić o statusy dzieci; od razu po
    // zakończeniu działania można usunąć ich wpisy w tablicy procesów
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
        return 1;
    }
	
    while (true) {
        
		cout << "Oczekiwanie na połączenie" << endl;
		if (listen (gniazdko, 10) == -1) {
			close (gniazdko);
			exit (EXIT_FAILURE);
		}
		
		// tworzenie połączenia z klientem
		int polaczenie = -1;
		if ((polaczenie = accept(gniazdko, (struct sockaddr *) &adresKlienta, &adresKlienta_len)) == -1) {
			perror("accept");
			exit (EXIT_FAILURE);
		} else {
			char buf[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, &adres.sin_addr, buf, sizeof(buf)) == NULL) {
				perror("inet_ntop");
				strcpy(buf, "???");
			}
			cout << "Nowe połączenie " << buf << ":" << (unsigned int) ntohs(adres.sin_port) << endl;
		}
		
		// ustawianie timeoutu serwera
		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		
		if (setsockopt (polaczenie, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout)) == -1) {
			perror ("setsockopt");
			exit (EXIT_FAILURE);
		}
		
		// dzielimy proces
        pid_t pid = fork();
        if (pid == -1) {
            // instrukcje do wykonania w razie nieudanego forkowania
            perror("fork");
			exit (EXIT_FAILURE);
        } else if (pid == 0) {
            // instrukcje do wykonania w procesie-dziecku
            
			// obsługa klienta
            sumator(polaczenie, adresKlienta);
			
			// zamknij kopię połączenia dziecka
			if (close(polaczenie) == -1) {
				perror("close");
				exit (EXIT_FAILURE);
			}
			
			// zamknij kopię gniazdka dziecka
			if (close(gniazdko) == -1) {
				perror("close");
				exit (EXIT_FAILURE);
			}
			
			// zakończ proces
            exit(0);
        }
		
        // dziecko dostaje kopię połączenia dlatego rodzic go natychmiast zamyka
		if (close(polaczenie) == -1) {
			perror("close");
			exit (EXIT_FAILURE);
		}
	}
}