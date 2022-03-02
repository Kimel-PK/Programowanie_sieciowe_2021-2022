#include <stdbool.h>

int main () {
	
	const char buf[14] = "testowe slowo";
	
	if (drukowalne (buf)) {
		printf ("tak");
	} else {
		printf ("nie");
	}
	
	drukowalne_pointer (buf);
	
}

bool drukowalne (const char* buf) {
	
	int i = 0;
	
	while (buf[i] != '\0') {
		if (buf[i] < 32 && buf[i] > 126)
			return false;
	}
	
	return true;
}

bool drukowalne_pointer (const void* buf) {
	
	const char* p = buf;
	
	for (; *p != '\0'; p++) {
		if (*p < 32 && *p > 126)
			return false;
	}
	
	return true;
}