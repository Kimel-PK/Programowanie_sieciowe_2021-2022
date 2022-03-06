#include <stdbool.h>
#include <stdio.h>

bool drukowalne (const void* buf, int len) {
	
	const char* _buf = buf;
	
	for (int i = 0; i < len; i++) {
		if (_buf[i] < 32 || _buf[i] > 126)
			return false;
	}
	
	return true;
}

bool drukowalne_pointer (const void* buf, int len) {
	
	const char* p = buf;
	
	for (int i = 0; i < len; ++p, i++) {
		if (*p < 32 || *p > 126)
			return false;
	}
	
	return true;
}

int main () {
	
	const char buf[14] = "testowe slowo\0";
	// const char buf[16] = "testowe•slowo\0";
	
	printf ("Wersja bez wskaznikow:\n");
	
	if (drukowalne (buf, 13)) {
		printf ("Wszystkie znaki w ciagu '%s' sa drukowalne\n\n", buf);
	} else {
		printf ("W ciagu '%s' wystepuja niedrukowalne znaki\n\n", buf);
	}
	
	printf ("Wersja ze wskaznikami:\n");
	
	if (drukowalne_pointer (buf, 13)) {
		printf ("Wszystkie znaki w ciagu '%s' sa drukowalne\n\n", buf);
	} else {
		printf ("W ciagu '%s' wystepuja niedrukowalne znaki\n\n", buf);
	}
	
}