#include <stdbool.h>
#include <stdio.h>

bool drukowalne (const char* buf) {
	
	for (int i = 0; buf[i] != '\0'; i++) {
		if (buf[i] < 32 || buf[i] > 126)
			return false;
	}
	
	return true;
}

bool drukowalne_pointer (const char* buf) {
	
	for (; *buf != '\0'; ++buf) {
		if (*buf < 32 || *buf > 126)
			return false;
	}
	
	return true;
}

int main () {
	
	const char buf[14] = "testowe slowo\0";
	// const char buf[16] = "testowe•slowo\0";
	
	printf ("Wersja bez wskaznikow:\n");
	
	if (drukowalne (buf)) {
		printf ("Wszystkie znaki w ciagu '%s' sa drukowalne\n\n", buf);
	} else {
		printf ("W ciagu '%s' wystepuja niedrukowalne znaki\n\n", buf);
	}
	
	printf ("Wersja ze wskaznikami:\n");
	
	if (drukowalne_pointer (buf)) {
		printf ("Wszystkie znaki w ciagu '%s' sa drukowalne\n\n", buf);
	} else {
		printf ("W ciagu '%s' wystepuja niedrukowalne znaki\n\n", buf);
	}
	
}