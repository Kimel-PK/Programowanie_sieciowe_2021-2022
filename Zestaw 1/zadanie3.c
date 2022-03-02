#include <stdbool.h>

int main () {
	
	const char buf[14] = "testowe slowo";
	
	if (drukowalne (buf, 14)) {
		printf ("tak");
	} else {
		printf ("nie");
	}
	
	drukowalne_pointer (buf, 14);
	
}

bool drukowalne (const void* buf, int len) {
	
	const char* _buf = buf;
	
	for (int i = 0; i < len; i++) {
		if (_buf[i] < 32 && _buf[i] > 126)
			return false;
	}
	
	return true;
}

bool drukowalne_pointer (const void* buf, int len) {
	
	const char* p = buf;
	
	for (; p < p + len; p++) {
		if (*p < 32 && *p > 126)
			return false;
	}
	
	return true;
}