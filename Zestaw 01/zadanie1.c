#include <stdio.h>

int main () {
	
	int liczby[50];
	
	int odczyt = 1;
	int i = 0;
	
	for (; i < 50 && odczyt != 0; i++) {
		scanf("%d", &odczyt);
		liczby[i] = odczyt;
	}
	
	printf("\nLiczby większe niż 10 ale mniejsze niż 100:\n");
	
	for (int j = 0; j < i; j++) {
		if (liczby[j] > 10 && liczby[j] < 100)
			printf ("%d\n", liczby[j]);
	}
	
}