//#include "set.h"
#include <stdio.h> 
//#include <stdin.h> 
#include <stdlib.h>
#include <string.h>


typedef struct bitset{
	unsigned char *bitVector;
	int sizeBit;
	int sizeByte;
}bitset;

bitset *mySet(int size){
	bitset *p;
	 p = malloc(sizeof(char));
	p->sizeBit = size;

	if(p->sizeBit % 8 == 1){
		p->sizeByte = size /8 + 1;
	}else{
		p->sizeByte = size /8 ;
	}


	p->bitVector = calloc(p->sizeByte , 8);
	return p;
}

int bitset_lookup(bitset *this, int item){
	bitset *p;
	int byte_index = item / 8;
	int bit_index = item % 8;
	int number = (byte_index &  bit_index << 1 ) ;

	if(number < 0){
		return -1;
	}else if(number == 0){
		return 0;
	}else{
		return 1; 
	}

}



int bitset_add(bitset *this, int item){

	int byte_index = item / 8;
	int bit_index = item % 8; 

	int lookup = bitset_add(this , item);

	if(lookup == -1){
		return 0;
	}else if(lookup == 1){
	}else{
		this->bitVector[item] = 1;
	}
	 return 1;
}









// int main(){
// 	const int MAX_STRING = 256;
// 	char input1[MAX_STRING];
// 	char input2[MAX_STRING];
// 	printf("Enter first set\n");
// 	fgets(input1 , MAX_STRING, stdin);
// 		printf("Enter second set\n");
// 	fgets(input2 , MAX_STRING, stdin);

// 	bitset *str1 = mySet(256); 
// 	bitset *str2 = mySet(256);

// 	int i;
// 	for(i=0; i<strlen(input1); i++){
// 		bitset_add(str1 , input1[i]);
// 	}


// 	int j;
// 	for(j=0; j<strlen(input2); j++){
// 		bitset_add(str2 , input2[j]);
// 	}



// 	int k;
// 	for(k=0; k<256; k++){
// 		if(bitset_lookup(str1, k)){
// 		printf("%c\n", str1[k]);
// 		}
// 	}

// 	return 0;
// }


