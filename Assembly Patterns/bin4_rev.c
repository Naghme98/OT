#include <stdio.h>

int main() {

	int input;
	printf("Enter an integer: ");
	scanf("%d", &input);
	
	if (!(input&1))
    		printf("%d is even.",input);
	else
    		printf("%d is odd.",input);


	return 0;
}


