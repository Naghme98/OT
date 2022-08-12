#include <stdio.h>

int main() {

	int i = 0;
	int arr[21];
	for(i; i<=19; i++){
	    arr[i] = i*2;
	}

	for(i=0; i<=19; i++){
	    printf("a[%d]=%d\n", i,arr[i]);
	}

	return 0;
}
