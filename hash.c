#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


 typedef struct node{
 	char *data;
 	struct node *next;
 }node;


typedef struct hashtable{
	int size;
	struct node **table;
}hashtable;


// construct a new hash table with size elements
 hashtable *hashtable_new(int size){
	hashtable *hashtable1;
	hashtable1 = malloc(sizeof(struct hashtable));
	//hashtable1 -> table = calloc(size, sizeof(node));
	//hashtable1 = calloc(size, sizeof(struct node) );
	hashtable1 -> table = malloc(sizeof(node)* size);

	  for(int i=0; i< size; i++){
	  	hashtable1->table[i] = NULL;
	
	  }	

	hashtable1->size = size;

	return hashtable1;
}



// Hashing function
unsigned hash_string(hashtable *this, char * str){
  unsigned hash = 0;
  for ( int i = 0; str[i] != '\0'; i++ ) {
    hash = hash * 37 + str[i];
  }
  //printf("Hashvalue %u\n", hash);
  return hash;
}


// insert string into the hash table, no effect if it's already there
void insert_hash(struct hashtable *this, char *item){
	//if(lookup(this,item) == 1)
		//return;
	unsigned hashValue = hash_string(this, item);
	hashValue = hashValue % this->size; 
	if(this->table[hashValue] == NULL){
		node *n = malloc(sizeof(node));
		n->data= item;
		n->next = malloc(sizeof(node));
		n->next->data = NULL;
		this->table[hashValue] = n;
	 }else{
	 	printf("Tryint to insert: %s\n", item);
	 	node *m = this->table[hashValue];
	 	while(m->next->data != NULL){
	 		m = m->next;
	 	}
	 	m = m->next;
	 	m->data = item;
	 	m->next = malloc(sizeof(node));
	 	m->next->data = NULL;
	 }

}

void print_hashtable(hashtable *this){
	for(int i=0; i < this->size; i++){
		if(this->table[i] == NULL){
			printf("<Empty>\n");
		}else{
			//printf("<Not Empty>");
			node *p;
			p = this->table[i]; 
			//printf("------> %s" , p->data);
				printf("------> %s\n", p->data );
			if(p->next != NULL && p->next->data != NULL){
				printf("------> %s , =====> %s\n" , p->data , p->next->data);
				//printf("-=====> %s\n" , p->next->data);
			}
		}
	}
}


// return 1 if string is already in table, 0 otherwise
int lookup(struct hashtable *this, char *item){
	unsigned hashValue= hash_string(this,item);
	hashValue = hashValue % this->size;
	if(this->table[hashValue] == NULL){
		return 0;
	}else{
		node *u = this->table[hashValue];
		while(u->next != NULL){
			int comparison = strcmp(item , u->data);
			if(comparison == 0)
				return 1;
			u = u->next;
		}
		return 0;
	}
}

// remove string from the hash table, no effect if not in table
int remove_item(struct hashtable * this, char * item){
	unsigned hashValue= hash_string(this,item);
	hashValue = hashValue % this->size;
	if(this->table[hashValue] == NULL){
		return 0;
	}else{
		node *u = this->table[hashValue];
		while(u->next != NULL){
			int comparison = strcmp(item , u->data);
			if(comparison == 0){
				u->data = NULL;
				return 1;
			}
			u = u->next;
		}
		return 0;
	}
}




int main(int argc, char *argv[]){
	// constructing hash table
	hashtable *hash1 = hashtable_new(10);
	// Getting input
	//int comparison = -1;
	//char symbol[1];
	//strcpy(symbol, "=");
	for(int i=1; i<argc; i++){
		printf("%s\n", argv[i]);
		if(strcmp(argv[i], "+") == 0){
			insert_hash(hash1 , argv[i+1]);
		}else if(strcmp(argv[i], "-") == 0){
			remove_item(hash1, argv[i+1]);

		}else if(strcmp(argv[i], "=") == 0){
			print_hashtable(hash1);
		}

	}

	//print_hashtable(hash1);
	//remove_item(hash1, argv[1]);
	//printf("Modified version of the table after deleting : \n");
	//fprint_hashtable(hash1);


	return 0;


}