#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct node{

	int freq;
	int is_leaf;
	union{
		struct{
			struct node *left;
			struct node *right;

		}compound;

		char c;
	}u;

};


struct node *create_compound_node(struct node *p , struct node *p2){
	struct node *result;
	result = malloc(sizeof(struct node));
	result->freq = p->freq + p2->freq;
	result->is_leaf =0;
	result->u.compound.left = p;
	result->u.compound.right = p2;
	return result;
};

int find_lowest_freq(struct node **list, int size){
	int i;
	int low =0;
	for(i=0; i<size; i++){
		if(list[i]->freq < list[low]->freq){
			low = i;
		}
	}
	return low;
}



struct node* build_huffman_tree(int *freqs, int nfreqs){
			//fprintf(stder, "it comes here 1");
	struct node **list;
	list = malloc( sizeof(struct node) * nfreqs);
			//printf("it comes here");
	int i;
	for(i=0; i<nfreqs; i++){
		struct node *p = malloc( sizeof(struct node));
		p->freq = freqs[i];
		p->is_leaf = 1;
		p->u.c = (char) i;
		list[i]=p;
	}

	int size = nfreqs;
	while(size > 1){
		int smallest, smallest2;
		struct node *p, *p2, *comp;
		smallest = find_lowest_freq(list , size);
		p = list[smallest];
		list[smallest] = list[size-1];
		size--;
		smallest2 = find_lowest_freq(list,size);
		p2 = list[smallest2];
		comp = create_compound_node(p,p2);
		list[smallest2] = comp;
	}
	return list[0];
};

void print_huffman_node(struct node *my_node, char *perfix){
	if(!my_node -> is_leaf){
		char temp[strlen(perfix)+1];
				// go left
		strcpy(temp,perfix);
		strcat(temp,"0");
		print_huffman_node(my_node->u.compound.left,temp);
				// go right
		char temp_second[strlen(perfix)+1];
		strcpy(temp_second,perfix);
		strcat(temp_second,"1");
		print_huffman_node(my_node->u.compound.right, temp_second);

	}else{

		// print node
		if(my_node->u.c >= 32 && my_node->u.c <= 127){
			char temp[strlen(perfix)+1];
			strcpy(temp,perfix);
			strcat(temp,"\0");
				//fprintf(stderr, "HERE1\n");
			printf("%c %s\n",my_node->u.c,temp);
		}

	}
}

int main(int argc, char ** argv){

	FILE * file;
	int *counts;
	counts = malloc(sizeof(int) * 256);
	char c;

  // for(int i=0; i<256; i++){
  // 	counts[i] = 0;
  // }

	if ( argc != 2 ) {
		fprintf(stderr, "Useage: huffman <filename>\n");
    exit(1);	    // exit with error code
}

file = fopen(argv[1], "r");
assert( file != NULL );
  c = fgetc(file);	// attempt to read a byte

  while( !feof(file) ) {
    //printf("%c", c);
  	c = fgetc(file);
  	int temp = (int)c;
  	counts[temp]++;
  }
  fclose(file);

 //  int  temp;
 //  for(c=fgetc(file); !feof(file); c=getc(file) ){
	// temp=(unsigned char)c;
	// counts[temp]++;
	// //printf("%d %c\n",counts[temp], temp);  	
 //  }
   // int i;
   // for(i=0; i<256; i++){
   // 	printf("%c %d\n", i , counts[i]);
   // }

  int xs;
  for(xs=0;xs<256;xs++){
  	if(counts[xs]==0){
  		counts[xs]=1;
  	}
  }
  
  struct node *tree = build_huffman_tree(counts, 256);
  char *hi = "";
  print_huffman_node(tree,hi);

  return 0;  // exit without error code
}