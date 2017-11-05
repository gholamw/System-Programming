
#include "mySet.c"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
int main(){
	const int MAX_STRING = 256;
	char input1[MAX_STRING];
	char input2[MAX_STRING];
	printf("Enter first set\n");
	fgets(input1 , MAX_STRING, stdin);
		printf("Enter second set\n");
	fgets(input2 , MAX_STRING, stdin);

	bitset *str1 = mySet(256); 
	bitset *str2 = mySet(256);

	int i;
	for(i=0; i<strlen(input1); i++){
		bitset_add(str1 , input1[i]);
	}


	int j;
	for(j=0; j<strlen(input2); j++){
		bitset_add(str2 , input2[j]);
	}



	int k;
	for(k=0; k<256; k++){
		if(bitset_lookup(str1, k)){
		printf("%c\n", str1[k]);
		}
	}

	return 0;
}