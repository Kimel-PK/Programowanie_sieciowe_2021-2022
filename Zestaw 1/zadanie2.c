#include <stdio.h>

int main () {
	
	int liczby[50];
	
	int odczyt = 1;
	int* p = NULL;
	
	for (p = liczby; p < liczby + 50 && odczyt != 0; ++p) {
		scanf("%d", &odczyt);
		*p = odczyt;
	}
	
	printf("\nLiczby większe niż 10 ale mniejsze niż 100:\n");
	
	for (int* r = liczby; r < p; ++r) {
		if (*r > 10 && *r < 100)
			printf ("%d\n", *r);
	}
	
}