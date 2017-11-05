# include <stdio.h>
# include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
//#include <errno.h>


int const max = 254;    // Global variable


// Defining type of stack(Abstract Data Type)
typedef struct stack{
	double array[max];
	int top;

}stack;



// A function pushs items to stack
void push(stack *p, double element){
	printf("Push ");
	

	if(p -> top >= max){
		printf("Your stack is full, please pop some items\n");
	}else{
		p->array[p->top++] = element;
		printf("  %f\n", element);
	}


	
}




// A function pops itmes off the stack
double pop(stack *p){
	printf("pop ");
	double temp = 0;
	if(p->top == 0){
		
		printf("Your stack is empty, please push some items\n");
	}else{
		p -> top--;
		temp = p->array[(p->top)];
		printf("  %f\n", temp);
	}

	
	return temp;
}




// A function which checks if stack is empty
int isEmpty(stack *p){
	if(p->top == 0){
		return 1;
	}else{
		return 0;
	}
}



// A function that converts a char to int
double convert(char result [] ){
	char t = result[0];
	double conversion = atof(result);
	return conversion;
}





// A function to split string when find a space. Returns the index
// of the start of the next character within the string
int tokenise(char str[], int start, char result[]){
	int i=start;
	
		while(str[i] == ' '){
			i++;
		}

		if(str[i]=='\0'){
			return -1;
		}

		int index =0;
		while(str[i] != ' ' && str[i]!='\0'){
		result[index] = str[i];
		index++;
		i++;
		}
	
	result[index] = '\0';
	return (i + 1);
}







// Main function
int main(){
	const int MAX_STRING = 256;
	char buffer [MAX_STRING];
	char result [MAX_STRING];
	stack *c = malloc(sizeof(stack));
	c -> top = 0;
	int start =0;
	double elementToStore =0;
	fgets(buffer,MAX_STRING,stdin);
	start = tokenise(buffer,0,result);
	//convert(result);
	 elementToStore = convert(result);
	 push(c , elementToStore);
	while(start != -1){
		double res =0;
		//printf("%s\n", result);
	    if(result[0] == '+'){
				//printf("\nIts plus char\n");
				double pop1 = pop(c);
				double pop2 = pop(c);
				res = pop1 + pop2;
				push(c, res);
		}else if(result[0] == '-' && (result[1] == '\0' || result[1] == ' ' || result[1] == '\n') ){
				double pop1 = pop(c);
				double pop2 = pop(c);
				res = pop1 - pop2;
				push(c, res);
		}else if(result[0] == '/'){
				double pop1 = pop(c);
				double pop2 = pop(c);
				res = pop1 / pop2;
				push(c, res);
		}else if(result[0] == '*'){
				double pop1 = pop(c);
				double pop2 = pop(c);
				res = pop1 * pop2;
				push(c, res);

		}else{


			elementToStore = convert(result);
			if(elementToStore != 0)
				push(c , elementToStore);

		}
			    start = tokenise(buffer,start,result);

	}
}



