#include "20151529.h"

int lastaddress = (-1);
int opflag = 0;
int symflag = 0;
int base_addressing=0;
int subrout_num;
int direct_num;


int main(void) {
	char*inpcmnd, *correctinp, *comnd, *argbuf[3];
	int comndnum, argnum = 0, index = 0, j = 0, k = 0, succ;
	int breakflag = 0;// breakflag is bigger than 0 when input is invalid
	int blankflag = 0;//refers if blank is included in input
	LinkedList *Llist = (LinkedList *)malloc(sizeof(LinkedList));
	Hash *optable = (Hash *)malloc(sizeof(Hash) * 20);
	Hash *symtab = NULL;
	Hash *newSymtab = NULL;// (Hash *)malloc(sizeof(Hash) * 26);
	Assemcode *assemcode = (Assemcode *)malloc(sizeof(Assemcode) * 200);
	unsigned char *mem = (unsigned char *)malloc(sizeof(unsigned char) * 0xFFFFF);
	Llist->cur = Llist->first = NULL;

	while (1) {
		printf("sicsim> ");
		breakflag = 0; blankflag = 0; index = 0; j = 0; argnum = 0; k = 0;
		inpcmnd = (char *)malloc(sizeof(char)*(MAX_SIZE_COMND));
		correctinp = (char *)malloc(sizeof(char)*(MAX_SIZE_COMND));
		while ((inpcmnd[index] = getchar()) != '\n') {
			correctinp[k++] = inpcmnd[index]; //for history function, if inpcmnd is proper input,
			index = letterfilter(inpcmnd, index, &blankflag, &breakflag); //return the next index for input command
			if (breakflag) break;
			if (k >= 49) {
				printf("Input is too long\n"); breakflag++; break;
			}
		}
		if (breakflag) {
			breakflag = 0;
			continue;
		}
		//Trimming input to String--------------------------------------------------------------------------------
		if (blankflag) inpcmnd[index - 1] = '\0'; //dealing with Enter input after blank
		else    correctinp[k] = inpcmnd[index] = '\0'; //no blank but reassigning null instead Enter
		correctinp[k] = '\0';
		//putting null into the end of the string
		if (inpcmnd[index - 1] == ',') {
			printf("Not proper argument input\n");
			continue;
		}

		comnd = strtok(inpcmnd, " ");//assigning the address of the first command
		while ((argbuf[argnum] = strtok(NULL, " ")) != NULL)     (argnum)++;//checking if input line has arguments

		if (argnum) {//if argument exists, check if it is not the last, give NULL replacing ','
			for (j = 0; j < argnum; j++) {
				if (j == argnum - 1) { //check the last argument's last literal
					if (argbuf[j][strlen(argbuf[j]) - 1] == ',' || argbuf[j][strlen(argbuf[j]) - 1] == ' ') {
						printf("Not proper argument input, check ',' usage\n"); breakflag++; break;
					}
				}
				else {
					if (argbuf[j][strlen(argbuf[j]) - 1] != ',') {//check not the last argument's last literal
						printf("Not proper argument input, check ',' usage\n"); breakflag++; break;
					}
					else argbuf[j][strlen(argbuf[j]) - 1] = '\0';
				}
			}
		}
		if (breakflag) {
			breakflag = 0;
			continue;
		}
		if ((comndnum = checkright(comnd))) {//check it the input command is in the db
			succ = balancer(comndnum, argbuf, argnum, assemcode, Llist, optable, &symtab, &newSymtab, mem);
			if (succ) addLlist(Llist, NULL, &correctinp, 0);
			else printf("Invalid input for %s\n", comnd);
		}
		else printf("No command such as %s\n", comnd);
		free(inpcmnd);
		free(correctinp);
	}
}
