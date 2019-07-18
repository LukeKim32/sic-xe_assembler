#include "20151529.h"

int letterfilter(char *inpcmnd, int index, int *blankflag, int *breakflag) { //0~9, a~z, A~Z get all input
	int i = index;
	if (inpcmnd[i] == ' ' || inpcmnd[i] == '\t') {
		(*blankflag)++;
		if ((*blankflag) <= 1) {
			inpcmnd[i++] = ' ';
		}
	}
	else if (inpcmnd[i] == ',') {
		if ((*blankflag)) {
			inpcmnd[i - 1] = inpcmnd[i];
			(*blankflag) = 0;
		}
		else {
			inpcmnd[i + 1] = ' ';
			i += 2;
			(*blankflag)++;
		}
	}
	else {
		if (i == 1 && (*blankflag) > 0) inpcmnd[i - 1] = inpcmnd[i]; //if there is blank space before first word
		else i++;
		(*blankflag) = 0;
	}
	return i;
}

int balancer(int comndnum, char *arg[], int argnum, Assemcode *assemcode, LinkedList *hist, Hash *optable, Hash ** symtable, Hash **newSymtab, unsigned char *mem) {
	int proc = 0; int total_line = 0; int k;
	enum Command {
		H = 1, HELP, D, DiR, Q, QUIT, HI, HISTORY, DU, DUMP, E, EDiT, F, FILL, RESET, OPCODE, OPCODELIST, ASSEMBLE, TYPE, SYMBOL,
	};
	int locctr; Node *forfree, *nextforfree;

	switch (comndnum) {
	case H: case HELP:
		if (argnum) printf("No arguments allowed for help\n");
		else {
			help(); proc++;
		}
		break;
	case D: case DiR:
		if (argnum) printf("No arguments allowed for directory\n");
		else {
			dir(); proc++;
		}
		break;
	case Q: case QUIT:
		freemalloc(assemcode, hist, optable, *symtable, mem);
		exit(0);
		break;
	case HI: case HISTORY:
		if (argnum) printf("No argumnets allowed for history\n");
		else {
			history(hist); proc++;
		}
		break;
	case DU:        case DUMP:
		if (argnum > 2) printf("Too many arguments for \"dump\"\n");
		else {
			dump(arg, argnum, mem);
			proc++;
		}
		break;
	case E:  case EDiT:
		if (argnum == 2) {
			if (edit(arg, mem) == -1) break;
			else proc++;
		}
		else if (argnum < 2) printf("2 arguments are needed for \"edit\"\n");
		else printf("Too many arguments for \"edit\"\n");
		break;
	case F: case FILL:
		if (argnum == 3) {
			if (fill(arg, mem) == -1) break;
			else proc++;
		}
		else if (argnum < 3) printf("3 arguments are needed for \"fill\"\n");
		else printf("Too many arguments for \"fill\"\n");
		break;
	case RESET:
		reset(mem);
		proc++;
		break;
	case OPCODE:
		if (!opflag) {
			if (ophashMake(optable)) opflag++; //when opcode hash table is properly made
		}
		if (opflag) {
			if (argnum) {
				if (opcodesearch(arg, optable, NULL, 1)) proc++;
			}
			else printf("Input opcode lookin for\n");
		}
		break;
	case OPCODELIST:
		if (!opflag) {
			if (ophashMake(optable)) opflag++; //when opcode hash table is properly made
		}
		if (opflag) {
			if (!argnum) {
				if (opcodelist(optable))  proc++;
			}
			else printf("No arguments allowed for opcodelist\n");
		}
		break;
	case ASSEMBLE:
		if (argnum == 1) {
			if (!opflag) {
				if (ophashMake(optable)) opflag++; //when opcode hash table is properly made
			}
			if (opflag) {
				if ((locctr = assemPassOne(arg[0], assemcode, newSymtab, optable))) {  //returns locctr
					if ((total_line = assemPassTwo(assemcode, newSymtab, optable)) != 0) { //assemble succeed!
						if (symflag) {
							for (k = 0; k < 26; k++) { //symbol table memory free
								nextforfree = forfree = (*symtable + k)->next;
								if (forfree != NULL) {
									while (forfree->next != NULL) {
										nextforfree = forfree->next;
										free(forfree);
										forfree = nextforfree;
									}
								}
								free(forfree);
							}
							symflag--;
						}
						*symtable = *newSymtab;
						writeListFile(assemcode, arg[0], total_line);
						writeObjectCode(assemcode, arg[0], total_line, locctr);
						symflag++;
						proc++;
					}
				}
				else {
					printf("Assembly code Error\n");
				}
			}
		}
		else printf("Argument error for assembly function\n");
		break;
	case TYPE:
		if (argnum == 1) {
			if (type(arg[0]) == 1) proc++;
		}
		else {
			printf("Type command needs an argument\n");
		}
		break;
	case SYMBOL:
		if (argnum == 0) {
			if (symflag) {
				symtabPrint(*symtable);
				proc++;
			}
			else printf("no symbol table .asm file must be assmebled first\n");
		}
		else printf("No arguments allowed for symbol\n");
		break;
	}
	return proc;
}

void freemalloc(Assemcode *assemcode, LinkedList *hist, Hash * optable, Hash * symtab, unsigned char *mem) {
	Node *forfree, *nextforfree;
	int i;
	if (hist->first != NULL) {
		nextforfree = forfree = hist->first; //history memory free
		while (forfree->next != NULL) {
			nextforfree = forfree->next;
			free(forfree);
			forfree = nextforfree;
		}
		free(forfree);
		free(hist);
	}
	if (opflag) {
		for (i = 0; i < 20; i++) { //hash table memory free
			nextforfree = forfree = (optable + i)->next;
			if (forfree != NULL) {
				while (forfree->next != NULL) {
					nextforfree = forfree->next;
					free(forfree);
					forfree = nextforfree;
				}
			}
			free(forfree);
		}
		free(optable);
	}
	if (symflag) {
		for (i = 0; i < 26; i++) { //symbol table memory free
			nextforfree = forfree = (symtab + i)->next;
			if (forfree != NULL) {
				while (forfree->next != NULL) {
					nextforfree = forfree->next;
					free(forfree);
					forfree = nextforfree;
				}
			}
			free(forfree);
		}
		free(symtab);
	}
	free(mem);
	free(assemcode);
}

int checkright(char* inp) {
	char comnd1[8][9] = { "h","help","d","dir","q","quit","hi","history" };
	char comnd2[12][11] = { "du","dump","e","edit","f","fill" ,"reset","opcode","opcodelist","assemble", "type", "symbol" };
	int i;
	int flag = 0;//To jump another for loop if command is found among comnd1

	for (i = 0; i < 8; i++) { //check if input command belongs to cmmand group1
		if (!strcmp(inp, comnd1[i])) {
			flag++; break;
		}
	}
	if (!flag) {//comnd1 doesnt have inp, so check command group2
		for (; (i - 8) < 12; i++) {
			if (!strcmp(inp, comnd2[i - 8]))        break;
		}
	}
	return i + 1; //i is an index of commands, each commands refers to 1 to 15 in order, as 1~8 commands are shell related, 9~14 are memory related
	//if i > 13, input is invalid. else returns the index
}

void addLlist(LinkedList *hist, Hash *table, char **inpcomnd, int mode) {
	Node *newNode = (Node *)malloc(sizeof(Node));
	Node *current;
	int i = 2;
	int tableindex;

	if (mode) { //mode=1, hashtable add
		strcpy(newNode->comnd, inpcomnd[1]); //opcode name add
		newNode->seq = converthex(inpcomnd[0]); //opcode hex num add
		strcpy(newNode->format, inpcomnd[2]); // opcode format add
		newNode->next = NULL;
		tableindex = ophashindex(inpcomnd[1]); //searching for opcode's hash index
		if ((table + tableindex)->next == NULL) { //if the node is first node of the table's index
			(table + tableindex)->next = newNode;
		}
		else { //if not the first node of the table's index
			current = (table + tableindex)->next;
			while (current->next != NULL) current = current->next;
			current->next = newNode;
		}
	}
	else { //mode=0, history linkedlist add
		strcpy(newNode->comnd, *inpcomnd);
		newNode->next = NULL;

		if (hist->first == NULL) {//first node added
			newNode->seq = 1;
			hist->first = newNode;
		}
		else {//not the first
			current = hist->first;
			while (current->next != NULL) {
				current = current->next;
				i++;
			}
			current->next = newNode;
			newNode->seq = i;
		}
	}
}

void help(void) {
	printf("h[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
}

void dir(void) {
	struct dirent **list;
	int numFiles;
	int i;
	struct stat finfo;

	if ((numFiles = scandir(".", &list, NULL, NULL)) == -1) {
		return;
	}
	for (i = 0; i < numFiles; i++) {
		if (!strcmp(list[i]->d_name, ".") || !strcmp(list[i]->d_name, "..")) continue;
		printf("%s", list[i]->d_name);
		stat(list[i]->d_name, &finfo);
		if (S_ISDIR(finfo.st_mode)) printf("/");
		else if (finfo.st_mode & S_IEXEC) printf("*");
		printf(" ");
	}
	printf("\n");
	for (i = 0; i < numFiles; i++) free(list[i]);
	free(list);
}

void history(LinkedList *hist) {
	Node* current = hist->first;
	if (current == NULL) printf("No instruction history\n");
	else {
		while (current != NULL) {
			printf("%d %s\n", current->seq, current->comnd);
			current = current->next;
		}
	}
}

void dump(char *arg[], int argnum, unsigned char *mem) {
	int range[2];
	int i, error = 0;
	int outbound = 0xFFFFF;

	switch (argnum) {
	case 0: //if there are no arguments in dump command
		if (lastaddress == outbound || lastaddress == (-1)) range[0] = lastaddress = 0; //check if the latest address at the end or dump hasnt been executed so far
		else range[0] = lastaddress + 1; //give the next index from the last address
		if ((range[1] = (range[0] + 159)) > outbound) range[1] = outbound; // allocate the outter range to pring 160 bytes
		break;
	case 1: //if there is a single argument in dump command
		range[0] = converthex(arg[0]);
		if ((range[1] = range[0] + 159) > outbound) range[1] = outbound;
		if (range[0]<0 || range[0]>outbound) { //check if given start range is within the boundary
			printf("Address is not within the boundary!\n");
			error++;
		}
		break;
	case 2:
		range[0] = converthex(arg[0]);
		range[1] = converthex(arg[1]);
		for (i = 0; i < 2; i++) {  //check if given start & end range is within the boundary
			if (range[i]<0 || range[i]>outbound) { //address valid check
				printf("%d-st Argument address is not within the boundary(0x00000-0xFFFFF)\n", i); error++; error++;
			}
		}
		if (range[0] > range[1]) { //check if given start range is bigger than given end range
			printf("Start address cannot be bigger than end address\n"); error++;
		}
	}
	if (!error) dumpprint(range, mem); //if theres no exception in input, print range
}

void dumpprint(int *range, unsigned char *mem) {
	int i, j;
	int refinerange = range[1] / 16;
	if (range[1] % 16) refinerange++; //if outter range has a leftover after divided by 16, we have to consider the line
	for (j = (range[0] / 16); j < refinerange; j++) {
		printf("%05X ", (j) * 16); //print the memory address in hexadecimal 5 letters
		for (i = 0; i < 16; i++) { //print the value in hex
			if ((i + (j * 16)) >= range[0] && i + (j * 16) <= range[1]) printf("%02X ", (mem[i + (j * 16)]));
			else printf("   ");
		}
		printf(";");
		for (i = 0; i < 16; i++) { //print the value into corresponding character
			if (mem[i + (j * 16)] >= 0x20 && mem[i + (j * 16)] <= 0x7E) printf("%c", mem[i + (j * 16)]);
			else printf(".");
		}
		printf("\n");
	}
	lastaddress = range[1]; //keeping trace of lastaddress
}

int converthex(char *arg) { //convert character int hexadecimal, and handles input exception for command and hex inp
	int tot = 0;
	int len = strlen(arg);
	int i, j = 0, hexstart = 0;
	int argbuf;

	if (arg[0] == '0') {
		if (arg[1] == 'x' || arg[1] == 'X') hexstart = 2; //if the argument input starts with 'Ox*****' or 'OX*****'
	}
	for (i = len; i > hexstart; i--) {
		if (arg[i - 1] >= 48 && arg[i - 1] <= 57) argbuf = arg[i - 1] - 48; //'0'~'9' process
		else if (arg[i - 1] >= 65 && arg[i - 1] <= 70) argbuf = arg[i - 1] - 55; //'A'~'F' process, since 'A','a' starts from 10
		else if (arg[i - 1] >= 97 && arg[i - 1] <= 102) argbuf = arg[i - 1] - 87; //'a'~'f' process
		else { //if given argument is not the proper character input
			printf("Argument %s is not hexadecimal\n", arg);
			return -1;
		}
		tot += exponhex(j++)*(argbuf);
	}
	return tot;
}

int exponhex(int x) { //for exponential operation
	int i, res = 1;
	for (i = 1; i <= x; i++) res *= 16;
	return res;
}

int expondec(int x) { //for exponential operation
	int i, res = 1;
	for (i = 1; i <= x; i++) res *= 10;
	return res;
}

int edit(char *arg[], unsigned char *mem) {
	int addrs = converthex(arg[0]); //first argument is for memory address
	int val = converthex(arg[1]); //second argument is for value
	if (addrs == -1 || val == -1) return -1; //if input address or value consist of non-hexadecimal words
	if (addrs > 0xFFFFF) { //if address value is bigger than boundary
		printf("Input address is out of boundary\n");
		return -1;
	}
	if (val > 0xFF) { //if value is bigger than boundary
		printf("Input value cannot be bigger than 0xFF\n");
		return -1;
	}
	mem[addrs] = val;
	return 1;
}

int fill(char *arg[], unsigned char *mem) {
	int start = converthex(arg[0]); //first arg for start range
	int end = converthex(arg[1]); //second arg for end range
	int val = (converthex(arg[2])); //third arg for value
	int i;
	if (start == -1 || end == -1 || val == -1) return -1;//if input address or value consist of non-hexadecimal words
	if (start > 0xFFFFF || end > 0xFFFFF) { //if address value is bigger than boundary
		printf("Input address is not within the boundary\n");
		return -1;
	}
	if (start > end) {
		printf("Input start address cannot be bigger than end address\n");
		return -1;
	}
	if (val > 0xFF) { //if value is bigger than boundary
		printf("Input value cannot be bigger than 0xFF\n");
		return -1;
	}
	for (i = start; i <= end; i++) mem[i] = val;
	return 1;

}

void reset(unsigned char *mem) {
	memset(mem, 0, 1024 * 1024);
}


int ophashMake(Hash *table) {
	FILE *fp = fopen("opcode.txt", "r");
	char opnum[4], opname[10], opformat[4];
	char *opcode[3];
	int k = 0, succ = 0;

	if (fp != NULL) {
		for (k = 0; k < 20; k++) { //initializing the hash table pointer array
			(table + k)->data = k;
			(table + k)->next = NULL;
		}
		while (!feof(fp)) {
			opcode[0] = opnum; opcode[1] = opname; opcode[2] = opformat;
			if (fscanf(fp, "%s %s %s", opcode[0], opcode[1], opcode[2]) == EOF) break;
			addLlist(NULL, table, opcode, 1);
			succ++;
		}
		fclose(fp);
	}
	else {
		printf("File doesn't exist!\n");
	}
	return succ;
}

int opcodesearch(char *arg[], Hash *table, Node **opcode, int mode) { //search for argument(opcode) from hash table
	int tableindex = ophashindex(arg[0]);
	Node *current = (table + tableindex)->next; //first letter is the hash key, so set the index
	char arg_buf[10];
	unsigned int i;
	if (*arg[0] == '+') {
		for (i = 0; i < strlen(arg[0]); i++) {
			arg_buf[i] = arg[0][i + 1];
		}
		arg_buf[i] = '\0';
	}
	else strcpy(arg_buf, arg[0]);

	while (current != NULL) { //searching if theres same opcode command name in table
		if (strcmp(current->comnd, arg_buf)) current = current->next;
		else break;
	}

	if (current == NULL) {
		if (mode == 1) printf("There is no such opcode as %s\n", arg_buf);
		return 0;
	}
	else {
		if (mode == 1)printf("opcode is %X\n", current->seq);
		if (opcode != NULL) *opcode = current;
		return 1;
	}
}

int ophashindex(char *opname) {
	int hash_key = 0;
	unsigned int i = 0;
	if (opname[0] == '+') { //format4
		i = 1;
	}
	for (; i < strlen(opname); i++) {
		hash_key += opname[i];
	}
	return hash_key % 20;
}
int opcodelist(Hash *table) {
	int i;
	Node *current;
	if (opflag) { //if opcode hash table is already made
		for (i = 0; i < 20; i++) {
			current = (table + i)->next;
			printf("%d : ", i);
			if (current != NULL) {
				while (current->next != NULL) {
					printf(" [%s, %X]  ->", current->comnd, current->seq);
					current = current->next;
				}
				if (current != NULL) printf(" [%s, %X] ", current->comnd, current->seq);
			}
			printf("\n");
		}
		return 1;
	}
	else {
		printf("No opcode list initialized\n");  return 0;
	}
}


int assemPassOne(char filename[], Assemcode *assemcode, Hash ** newSymtab, Hash *optable) {
	FILE *fp = NULL;
	char *inp, *inp_buf; //for file input
	int index = 0; //for inp indexing
	int word_num = 0; //the number of words in a line
	char *word[5]; char word_buf[70];
	int blankflag, breakflag; int codeline = 0;
	char *line_end_check;
	int Locctr = 0; 	int firstflag = 1; int length = 0; int origin_length = 0;
	unsigned int i = 0; 	int k = 0; int resw = 0; 	int temp_byte = 0;
	Node* opcode; char symbol_buf[20];

	char file_name[20], file_exten[6], file_temp[30];
	strcpy(file_temp, filename);
	strcpy(file_name, strtok(file_temp, "."));
	strcpy(file_exten, strtok(NULL, "\0"));
	if (strcmp(file_exten, "asm") == 0) {
		fp = fopen(filename, "r");
	}
	else {
		printf("%s is not .asm file\n", filename);
		return 0;
	}

	if (fp != NULL) {
		*newSymtab = (Hash *)malloc(sizeof(Hash) * 26);
		for (k = 0; k < 26; k++) { //initializing the hash table pointer array
			(*newSymtab + k)->data = k;
			(*newSymtab + k)->next = (Node*)malloc(sizeof(Node)); //dummy node
			(*newSymtab + k)->next->next = NULL;
		}
		while (1) {

			inp = (char *)malloc(sizeof(char) * 200);
			if (feof(fp)) break;
			blankflag = breakflag = 0;
			index = 0; word_num = 0; i = 0; resw = 0; temp_byte = 0;
			subrout_num = 0;
			if (Locctr > 0xFFFFF) {
				printf("Line %d : Memory size limit passed! Boundary : 0x00000 ~ 0xFFFFF\n", 5 * (codeline + 1));
				return 0;
			}
			while (i < 4) {
				(assemcode + codeline)->code[i][0] = '\0'; //initialzing
				word[i++] = NULL;
			}
			//start reading from a file--------------------------------------------------
			while ((inp[index] = fgetc(fp)) != '\n') { //reading a line
				if (inp[index] == EOF) break;
				index = assemLetterFilter(inp, index, &blankflag, &breakflag); //refining the input
				if (breakflag) return 0;
			}

			//Trimming input to String--------------------------------------------------------------------------------
			if (blankflag) inp[index - 1] = '\0'; //dealing with Enter input after blank
			else inp[index] = '\0'; //no blank but reassigning null instead Enter
			if (inp[0] == '\n' || inp[0] == '\0') continue; //'\n'이 연달아 들어온 경우
			if (inp[0] == '.') {
				(assemcode + codeline)->comment = (char *)malloc(sizeof(char) * 100);
				strcpy((assemcode + codeline)->comment, inp);
				(assemcode + codeline)->assign_address = Locctr;
				(assemcode + codeline)->word_num = (-1);
				(assemcode + codeline)->line_number = 5 * (codeline + 1);
				(assemcode + codeline)->code[0][0] = '.';
				(assemcode + codeline)->code[0][1] = '\0';
				codeline++;
				free(inp);
				continue;
			}
			else {
				origin_length = strlen(inp); //원래 command line 길이 저장
				inp_buf = (char *)malloc(sizeof(char) * 200);
				strcpy(inp_buf, inp);
				strcpy((assemcode + codeline)->code[word_num], strtok(inp, " "));//copying first word of a line of .asm file
				while ((line_end_check = strtok(NULL, " ")) != NULL) { //copying rest of the words from a line of .asm file
					strcpy((assemcode + codeline)->code[++word_num], line_end_check);
					if (word_num > 3) {
						printf("Line %d : Too many arguments\n", 5 * (codeline + 1));
						free(inp); free(inp_buf);
						return 0;
					}
					if (strcmp(line_end_check, "BYTE") == 0 || strcmp(line_end_check, "WORD") == 0) { //BYTE, WORD선언 시 operand 조정
						for (k = 0, length = 0; k < 2; k++) length += strlen((assemcode + codeline)->code[k]);
						length += 2; //Operand 시작 주소까지 길이 조정
						word_num++;
						for (k = length, i = 0; k < origin_length; k++, i++) {
							(assemcode + codeline)->code[word_num][i] = inp_buf[k]; //operand에 string 그대로 넣어주기
						}
						(assemcode + codeline)->code[word_num][i] = '\0';
						(assemcode + codeline)->format = '5';
						break;
					}

				}
				free(inp_buf);
				if (breakflag) break;
				(assemcode + codeline)->word_num = word_num;
				(assemcode + codeline)->format = 0;
			}
			//complete reading from file ----------------------------------
			for (k = 0; k < word_num + 1; k++) {
				word[k] = ((assemcode + codeline)->code[k]);
			} //for comfortable use
			//first line for "start" --------------------
			if (firstflag) {
				firstflag--;
				if (strcmp(word[0], "START") == 0 || strcmp(word[1], "START") == 0) {
					if (strcmp(word[0], "START") == 0) {//no program name
						if ((assemcode + codeline)->code[1][0] != '\0') {
							if ((assemcode + codeline)->code[2][0] != '\0') {
								printf("Line %d : Start directiv cannot have more operands\n", 5 * (codeline + 1));
								free(inp); return 0;
							}
							Locctr = converthex(word[1]); //convert address(character) to hex
							strcpy((assemcode + codeline)->code[2], (assemcode + codeline)->code[1]); //moving START, and starting address to second, third code
						}
						else Locctr = 0; //No starting address in first line
						if (Locctr == -1) {
							printf("Line %d : Starting address Error\n", 5 * (codeline + 1));
							free(inp); return 0;
						}
						strcpy((assemcode + codeline)->code[1], (assemcode + codeline)->code[0]);
						word[0][0] = '\0'; //for Program name has not been input
					}
					else { //first word is program name
						if (strcmp(word[1], "START") == 0) {
							if ((assemcode + codeline)->code[2][0] != '\0') Locctr = converthex(word[2]); //convert address(character) to hex
							else Locctr = 0;  //No starting address in first line
						}
						else {//theres no "START" directive, so error
							printf("Line %d : No START directive in .asm code\n", 5 * (codeline + 1));
							free(inp);
							return 0;
						}
						if (Locctr == -1) {
							printf("Line %d : Starting address Error\n", 5 * (codeline + 1));
							free(inp); return 0;
						}
					}
					(assemcode)->line_number = 5;
					(assemcode)->assign_address = Locctr; //locctr가 character이므로 hex로 변환
					codeline++;
					continue;
				}
				else {
					Locctr = 0;
					(assemcode)->line_number = 5;
					(assemcode)->assign_address = 0;
				}
			}
			//Not the first line-------------------------

			if (opcodesearch(&word[0], optable, &opcode, 0) == 0) {//first word is not opcode = its label or symbol
				//만약 첫번째 열의 단어가 directive라면 오류이다
				if (strcmp(word[0], "WORD") == 0) {
					printf("Line %d : Symbol name is needed for WORD declaration\n", 5 * (codeline + 1));
					free(inp); return 0;
				}
				else if (strcmp(word[0], "BYTE") == 0) {
					printf("Line %d : Symbol name is needed for BYTE declaration\n", 5 * (codeline + 1));
					free(inp); return 0;
				}
				else if (strcmp(word[0], "RESW") == 0) {
					printf("Line %d : Symbol name is needed for RESW declaration\n", 5 * (codeline + 1));
					free(inp); return 0;
				}
				else if (strcmp(word[0], "RESB") == 0) {
					printf("Line %d : Symbol name is needed for RESB declaration\n", 5 * (codeline + 1));
					free(inp); return 0;
				}
				else if (strcmp(word[0], "START") == 0) {
					printf("Line %d : START cannot appear again\n", 5 * (codeline + 1));
					free(inp); return 0;
				}
				else if (strcmp(word[0], "BASE") == 0) {
					base_addressing = 1;
					(assemcode + codeline)->assign_address = Locctr; //assign_address가 -1이면 할당X
					strcpy(symbol_buf, (assemcode + codeline - 1)->code[2]);
					if ((assemcode + codeline - 1)->code[2][0] == '#') {
						for (i = 1; i < strlen((assemcode + codeline - 1)->code[2]); i++) {
							symbol_buf[i - 1] = (assemcode + codeline - 1)->code[2][i];
						}
						symbol_buf[i - 1] = '\0'; //rendering symbol buf for check if it is immediate addressing
					} //LDB #LENGTH \n BASE LENGTH
					if (strcmp(word[1], symbol_buf) == 0) {
						strcpy((assemcode + codeline)->code[2], (assemcode + codeline)->code[1]);
						strcpy((assemcode + codeline)->code[1], (assemcode + codeline)->code[0]);
						(assemcode + codeline)->code[0][0] = '\0';
						(assemcode + codeline)->line_number = 5 * (codeline + 1);
						codeline++;
						free(inp);
						continue;
					}
					else {
						printf("Line %d : Operand of BASE register is not correct\n", 5 * (codeline + 1));
						free(inp);
						return 0;
					}
				}
				else if (strcmp(word[0], "NOBASE") == 0) {
					strcpy((assemcode + codeline)->code[2], (assemcode + codeline)->code[1]);
					strcpy((assemcode + codeline)->code[1], (assemcode + codeline)->code[0]);
					(assemcode + codeline)->code[0][0] = '\0';
					(assemcode + codeline)->assign_address = Locctr;
					(assemcode + codeline)->line_number = 5 * (codeline + 1);
					codeline++;
					free(inp);
					continue;
				}
				else if (strcmp(word[0], "END") == 0) {
					(assemcode + codeline)->assign_address = -1; //assign_address가 -1이면 할당X
					strcpy((assemcode + codeline)->code[2], (assemcode + codeline)->code[1]);
					strcpy((assemcode + codeline)->code[1], (assemcode + codeline)->code[0]);
					(assemcode + codeline)->code[0][0] = '\0';
					break;
				}
				if (checkSymbol(*newSymtab, word[0], Locctr) == 0) { //이미 symbol table에 존재
					printf("Line %d : Symbol %s already exists in SYMTAB\n", 5 * (codeline + 1), word[0]);
					free(inp);
					return 0; // error, symbol redundancy
				}
				else { //symbol is properly added to table
					(assemcode + codeline)->assign_address = Locctr;
					if (opcodesearch(&word[1], optable, &opcode, 0) == 0) { //second word should be opcode //opcode가 아니라면 symbold declaration을 위한 directive일 수 있다.
						//Operand를 보고 Locctr 연산
						if (strcmp(word[1], "END") == 0) {
							printf("Line %d: END directive cannot have label\n", 5 * (codeline + 1));
							free(inp); return 0;
						}
						if (strcmp(word[1], "START") == 0) {
							printf("Line %d: START cannot appear again\n", 5 * (codeline + 1));
							free(inp); return 0;
						}
						if (strcmp(word[1], "WORD") == 0) { //Directive = WORD
							Locctr += 3;
							(assemcode + codeline)->format = '5'; //format for word and byte
						}
						else if (strcmp(word[1], "RESW") == 0) {
							for (k = 0, i = strlen(word[2]); i >= 1; i--, k++) {
								if (word[2][i - 1] < '0' || word[2][i - 1] > '9') {
									printf("Line %d : Not proper operand for RESW\n", 5 * (codeline + 1));
									free(inp); return 0;
								}
								resw += (word[2][i - 1] - 48)*expondec(k); //decimal
							}
							if (resw != 0) Locctr += 3 * resw;
							else {
								printf("Line %d : Not proper operand for RESW\n", 5 * (codeline + 1));
								free(inp); return 0;
							}
						}
						else if (strcmp(word[1], "BYTE") == 0) {	//들어온 BYTE input이 valid한지 check
							if (strlen(word[2]) > 3) {//check the minimum format C' ' or X' '
								if (word[2][0] == 'C') {//character input
									if (word[2][strlen(word[2]) - 1] == 39 && word[2][1] == 39) { // ' = 39 in ASCII
										Locctr += (strlen(word[2]) - 3); //BYTE 글자수만큼 locctr 연산
										(assemcode + codeline)->format = '5';
									}
								}
								else if (word[2][0] == 'X') {//hexadecimal input
									if (word[2][strlen(word[2]) - 1] == 39 && word[2][1] == 39) {
										for (i = 2; i < strlen(word[2]) - 1; i++) {
											word_buf[i - 2] = word[2][i];
											if (word[2][i] >= '0' && word[2][i] <= '9') {}
											else if (word[2][i] >= 'A' && word[2][i] <= 'F') {}
											else {
												printf("Line %d : Operand must be proper Hexadecimal format\n", 5 * (codeline + 1));
												return 0;
											}
										}
										word_buf[i - 2] = '\0';
									}
									else {
										printf("Line %d : Operand not in proper format\n", 5 * (codeline + 1));
										return 0;
									}
									if (breakflag) break;
									if (strlen(word_buf) % 2 == 1) Locctr += (strlen(word_buf) / 2) + 1;
									else Locctr += (strlen(word_buf) / 2);
									(assemcode + codeline)->format = '5';
								}
							}
							else {	//not proper code
								printf("Line %d : Not proper operand for BYTE\n", 5 * (codeline + 1));
								free(inp);
								return 0;
							}
						}
						else if (strcmp(word[1], "RESB") == 0) {
							for (k = 0, i = strlen(word[2]); i >= 1; i--, k++) {
								if (word[2][i - 1] < '0' || word[2][i - 1] > '9') {
									printf("Line %d : Not proper operand for RESW\n", 5 * (codeline + 1));
									free(inp); return 0;
								}
								resw += (word[2][i - 1] - 48)*expondec(k); //decimal
							}
							if (resw != 0) Locctr += resw;
							else {
								printf("Line %d : Not proper operand for RESB\n", 5 * (codeline + 1)); free(inp);
								return 0;
							}
						}
						else {
							breakflag++; //error, directive가 아니다.
							printf("Line %d : Invalid code input\n", 5 * (codeline + 1)); free(inp);
							return 0;
						}
					}
					else {//if opcode is found
						if (opcode->format[0] != 4)  (assemcode + codeline)->format = opcode->format[0];
						else (assemcode + codeline)->format = '4';
						Locctr = formatCal(word[1], opcode, Locctr); //format을 확인해서 Locctr확인
					} //줄에 address값 할당
				}
			}
			else { //first word is opcode
				if (opcode->format[0] != 4) (assemcode + codeline)->format = opcode->format[0];
				else (assemcode + codeline)->format = '4';
				(assemcode + codeline)->assign_address = Locctr;
				Locctr = formatCal(word[0], opcode, Locctr);//format을 확인해서 Locctr확인
			}		//Column 정리 : col 1 에는 label, symbol / col 2에는 opcode, directive / col 3에는 operand
			if (word_num != (-1)) {//comment가 아니면
				switch (word_num) {
				case 0: //if the code line has a single word
					strcpy((assemcode + codeline)->code[1], (assemcode + codeline)->code[0]);
					(assemcode + codeline)->code[0][0] = '\0';
					break;
				case 1: //if the code lin has two words
					strcpy((assemcode + codeline)->code[2], (assemcode + codeline)->code[1]);
					strcpy((assemcode + codeline)->code[1], (assemcode + codeline)->code[0]);
					(assemcode + codeline)->code[0][0] = '\0';
					break;
				case 2:
					if (word[1][strlen(word[1]) - 1] == ',') { // ',' existence means two operands
						if (strcmp(word[2], "X") == 0) (assemcode + codeline)->index = 1;
						strcpy((assemcode + codeline)->code[3], (assemcode + codeline)->code[2]);
						strcpy((assemcode + codeline)->code[2], (assemcode + codeline)->code[1]);
						strcpy((assemcode + codeline)->code[1], (assemcode + codeline)->code[0]);
						(assemcode + codeline)->code[0][0] = '\0';
					}
					break;
				case 3:
					if (strcmp(word[3], "X") == 0 && word[2][strlen(word[2]) - 1] == ',') {
						(assemcode + codeline)->index = 1;
					}
					break;
				}
			}
			(assemcode + codeline)->line_number = 5 * (codeline + 1);
			codeline++;
			free(inp);
		}
		if (strcmp((assemcode + codeline)->code[1], "END") != 0) { //last sentence must include END
			printf("Line %d : Last sentence must include END\n", 5 * (codeline + 1));
			free(inp);
			return 0;
		}
		(assemcode + codeline)->line_number = 5 * (codeline + 1);
		free(inp);
		fclose(fp);
		return Locctr;
	}
	else {
		printf("There's no file such as %s\n", filename);
		return 0;
	}
	free(inp);
	return 0;
}

void symtabPrint(Hash *symtab) {
	int k;
	Node *symprint;
	char *text_print = (char *)malloc(sizeof(char) * 300);
	char text_buf[20]; char text2_buf[20];
	text_print[0] = '\0';
	for (k = 25; k >= 0; k--) {
		if (k != 0) {
			symprint = (symtab + k)->next->next;
			while (symprint != NULL) {
				sprintf(text_buf, "	%s", symprint->comnd);
				strcat(text_print, text_buf);
				if (symprint->seq < 0x10000) 	sprintf(text2_buf, "	%04X\n", symprint->seq);
				else sprintf(text2_buf, "	%05X\n", symprint->seq);
				strcat(text_print, text2_buf);
				symprint = symprint->next;
			}
		}
	}
	printf("%s", text_print);
	free(text_print);
}

int formatCal(char word_opcode[], Node *opcode, int Locctr) {
	int result = Locctr;
	if (word_opcode[0] == '+') result += 4; //format 4
	else {
		switch (opcode->format[0]) {
		case '1': //format 1
			result += 1; break;
		case '2': //format 2
			result += 2; break;
		case '3': //format 3
			result += 3;
		}
	}
	return result;
}

int checkSymbol(Hash *symtab, char *word, int Locctr) {
	int hashindex = hashSymbol(word);
	char symbol_buf[20];
	Node *push_location = (symtab + hashindex)->next; //dummy node
	Node *current = push_location->next;
	Node *newNode;
	int compare; //compare the first one

	strcpy(symbol_buf, word);
	if (word[strlen(word) - 1] == ',') symbol_buf[strlen(word) - 1] = '\0';
	if (current == NULL) { //if there's no node
		newNode = (Node *)malloc(sizeof(Node));
		strcpy(newNode->comnd, symbol_buf);
		newNode->seq = Locctr; //assigning address
		newNode->next = NULL;
		push_location->next = newNode;
		return 1; //symbol add
	}
	else { // if there's a node on table
		while ((compare = strcmp(current->comnd, symbol_buf)) > 0) {
			push_location = current;
			current = current->next;
			if (current == NULL) break;
		}
		if (current == NULL) { //if there's no node
			newNode = (Node *)malloc(sizeof(Node));
			strcpy(newNode->comnd, symbol_buf);
			newNode->seq = Locctr; //assigning address
			newNode->next = NULL;
			push_location->next = newNode;
			return 1; //symbol add
		}
		if (compare == 0) return 0;
		else if (compare < 0) {
			newNode = (Node *)malloc(sizeof(Node));
			strcpy(newNode->comnd, symbol_buf);
			newNode->seq = Locctr; //assigning address
			newNode->next = current;
			push_location->next = newNode;
			return 1;
		}
		return 0;
	}
}

int hashSymbol(char *word) {
	int index = word[0] - 65;
	return index;
}

int assemLetterFilter(char *inpcmnd, int index, int *blankflag, int *breakflag) { //0~9, a~z, A~Z get all input
	int i = index;
	if (inpcmnd[i] == ' ' || inpcmnd[i] == '\t') {
		(*blankflag)++;
		if ((*blankflag) <= 1) {
			inpcmnd[i++] = ' ';
		}
	}
	else if (inpcmnd[i] == ',') {
		if ((*blankflag)) {
			inpcmnd[i - 1] = inpcmnd[i];
			(*blankflag) = 0;
		}
		else {
			inpcmnd[i + 1] = ' ';
			i += 2;
			(*blankflag)++;
		}
	}
	else if ((inpcmnd[i] >= '0' && inpcmnd[i] <= '9') || (inpcmnd[i] >= 'A' && inpcmnd[i] <= 'Z') || inpcmnd[i] == '#' || inpcmnd[i] == '+' || inpcmnd[i] == '@' || inpcmnd[i] == 39 || inpcmnd[i] == '.') {
		if (i == 1 && (*blankflag) > 0) inpcmnd[i - 1] = inpcmnd[i]; //if there is blank space before first word
		else i++;
		(*blankflag) = 0;
	}
	else if ((inpcmnd[i] >= 'a' && inpcmnd[i] <= 'z')) {
		(*breakflag)++;
		printf("Assembly file should not include lower case characters\n");
	}
	else {
		(*breakflag)++;
		printf("Invalid input, proper command needed\n");
	}
	return i;
}

int returnSymbol(Hash *symtab, char *word) {
	int hashindex;
	char symbol_buf[20];
	Node *current;
	unsigned int i;

	if (word[0] == '\0') return 0; // if theres no operand
	strcpy(symbol_buf, word);
	if (word[strlen(word) - 1] == ',') symbol_buf[strlen(word) - 1] = '\0';
	if (word[0] == '#' || word[0] == '@') {
		for (i = 1; i < strlen(word); i++) {
			symbol_buf[i - 1] = word[i];
		}
		symbol_buf[i - 1] = '\0';
	}
	hashindex = hashSymbol(symbol_buf);
	if (hashindex > 25 || hashindex < 0) return 0;
	current = (symtab + hashindex)->next->next;
	while (current != NULL) { //searching if theres same opcode command name in table
		if (strcmp(current->comnd, symbol_buf)) current = current->next;
		else break;
	}
	if (current == NULL) return 0; // no opcode
	else return current->seq;//Assigned address를 return //found
}

int assemPassTwo(Assemcode *assemcode, Hash **newSymtab, Hash * optable) {
	int codeline = 0;
	Node *opcode;
	char *pst_word[4]; char word_buf[64]; char temp[10];
	unsigned int i;
	int symbol_address, disp, base, obj_comnd;
	int pc = (assemcode)->assign_address; //시작 pc
	int n_flag = 0, i_flag = 0, x_flag = 0, b_flag = 0, p_flag = 0, e_flag = 0;
	int flagsum = 0, reg_value = 0;
	direct_num = 0;
	(assemcode)->object_code = 0;
	while (1) {
		memset(word_buf, '\0', 64);
		if (strcmp((assemcode + codeline)->code[1], "START") == 0) {
			codeline++; pc = (assemcode + 1)->assign_address;
			continue;
		}
		if (strcmp((assemcode + codeline)->code[1], "BASE") == 0) { //BASE or comment
			(assemcode + codeline)->object_code = 0;
			base_addressing = 1;
			codeline++;
			continue;
		}
		if (strcmp((assemcode + codeline)->code[1], "NOBASE") == 0) {
			(assemcode + codeline)->object_code = 0;
			base_addressing = 0;
			codeline++;
			continue;
		}
		if ((assemcode + codeline)->code[0][0] == '.') {
			(assemcode + codeline)->object_code = 0;
			codeline++;
			continue;
		}
		for (i = 0; i < 4; i++) {
			pst_word[i] = ((assemcode + codeline)->code[i]);
		} //for comfortable use
		n_flag = 0, i_flag = 0, x_flag = 0, b_flag = 0, p_flag = 0, e_flag = 0;
		if (pst_word[0][0] != '.') { //If the code line is not the comment
			if (opcodesearch(&pst_word[1], optable, &opcode, 0) == 0) { //if col 2 word is not opcode, then it has to be "BYTE" or "WORD"
				if (strcmp(pst_word[1], "BYTE") == 0 || (strcmp(pst_word[1], "WORD") == 0)) {
					if (strcmp(pst_word[1], "BYTE") == 0) {
						if (pst_word[2][0] == 'X') {
							if (pst_word[2][1] == 39 && pst_word[2][strlen(pst_word[2]) - 1] == 39) {
								for (i = 2; i < strlen(pst_word[2]) - 1; i++) {
									word_buf[i - 2] = pst_word[2][i];
									if (pst_word[2][i] >= '0' && pst_word[2][i] <= '9') {}
									else if (pst_word[2][i] >= 'A' && pst_word[2][i] <= 'F') {}
									else {
										printf("Line %d : Operand must be proper Hexadecimal format\n", 5 * (codeline + 1));
										return 0;
									}
								}
								word_buf[i - 2] = '\0';
							}
							else {
								printf("Line %d : Operand not in format\n", 5 * (codeline + 1));
								return 0;
							}
						}
						else if (pst_word[2][0] == 'C') {
							if (pst_word[2][1] == 39 && pst_word[2][strlen(pst_word[2]) - 1] == 39) {
								for (i = 2; i < strlen(pst_word[2]) - 1; i++) { //word[2][0]앞에서부터 그냥 바로 %X값으로 넣어주기!
									memset(temp, '\0', 10);
									if (i == 2) sprintf(word_buf, "%X", pst_word[2][i]);
									else sprintf(temp, "%X", pst_word[2][i]);
									strcat(word_buf, temp);
								}
							}
							else {
								printf("Line %d : Operand not in format\n", 5 * (codeline + 1));
								return 0;
							}
						}
						else {
							printf("Line %d : Operand not hexadecimal or character\n", 5 * (codeline + 1));
							return 0;
						}
						if (strlen(word_buf) > 60) {
							printf("Line %d : BYTE operand cannot be bigger than 60 letters for text record of objfile\n", 5 * (codeline + 1));
							return 0;
						}
						sprintf((assemcode + codeline)->objcode_string, "%s", word_buf);
						disp = 0;
					}
					else { //WORD
						disp = charToDecimal(pst_word[2]);
					}
					if (disp == -1) {
						printf("Line %d : Operand error\n", 5 * (codeline + 1));
						return 0;
					}
					(assemcode + codeline)->object_code = disp;
				}
				else { //ERROR! col2에 적절한 단어가 아님
					if (strcmp(pst_word[1], "RESW") == 0 || strcmp(pst_word[1], "RESB") == 0) {
						codeline++;
						continue;
					}
					if (strcmp(pst_word[1], "END") == 0) {
						break;
					}
					if (strcmp(pst_word[1], "BASE") == 0) {
						codeline++;
						base_addressing = 1;
						continue;
					}
					if (strcmp(pst_word[1], "NOBASE") == 0) {
						codeline++;
						base_addressing = 0;
						continue;
					}
					printf("Line %d : No opcode or BYTE, WORD\n", 5 * (codeline + 1));
					return 0;
				}
			}
			else {// if col 2 word is opcode!
				if (strcmp(pst_word[1], "JSUB") == 0) subrout_num++;
				if (opcode->format[0] == '1') {
					(assemcode + codeline)->object_code = opcode->seq;
				}
				else if (opcode->format[0] == '2') {
					if (pst_word[2][0] != '\0') {//at least a register
						obj_comnd = opcode->seq * 256;
						if (strcmp(pst_word[1], "TIXR") == 0 || strcmp(pst_word[1], "CLEAR") == 0 || strcmp(pst_word[1], "SVC") == 0) { //the format 2 opcodes that have a single operand
							if (pst_word[3][0] == '\0') { //only one register
								if (pst_word[2][0] == '#') { //레지스터, #숫자
									reg_value = charToDecimal(pst_word[2]);
									if (reg_value == (-1)) {
										printf("Line %d : %s is not a number\n", 5 * (codeline + 1), pst_word[3]);
										return 0;
									}
									if (reg_value > 0xF) {
										printf("Line %d : %s is too big, it should be within 4bits\n", 5 * (codeline + 1), pst_word[3]);
										return 0;
									}
								}
								else {
									reg_value = returnReg(pst_word[2]);
									if (reg_value == 10) { //not proper input of register
										printf("Line %d : Not a proper register %s\n", 5 * (codeline + 1), pst_word[2]);
										return 0;
									}
								}
								reg_value *= 16;
								(assemcode + codeline)->object_code = obj_comnd + reg_value;
							}
							else {
								printf("Line %d : only one operand is permitted for %s\n", 5 * (codeline + 1), pst_word[2]);
								return 0;
							}
						}
						else { //at least 2 operands
							if (pst_word[3][0] != '\0') { //should have 2 operands
								reg_value = returnReg(pst_word[2]);
								if (reg_value == 10) { //not proper input of register
									printf("Line %d : Not a proper register %s\n", 5 * (codeline + 1), pst_word[2]);
									return 0;
								}
								obj_comnd += reg_value * 16;
								if (pst_word[3][0] == '#') { //레지스터, #숫자
									reg_value = charToDecimal(pst_word[3]);
									if (reg_value == (-1)) {
										printf("Line %d : %s is not a number\n", 5 * (codeline + 1), pst_word[3]);
										return 0;
									}
									if (reg_value > 0xF) {
										printf("Line %d : %s is too big, it should be within 4bits\n", 5 * (codeline + 1), pst_word[3]);
										return 0;
									}
									(assemcode + codeline)->object_code = obj_comnd + reg_value;
								}
								else { // 레지스터, 레지스터
									reg_value = returnReg(pst_word[3]);
									if (reg_value == 10) { //not proper input of register
										printf("Line %d : Not a proper register %s\n", 5 * (codeline + 1), pst_word[3]);
										return 0;
									}
									(assemcode + codeline)->object_code = obj_comnd + reg_value;
								}
							}
							else {
								printf("Line %d : two operand is needed for %s\n", 5 * (codeline + 1), pst_word[2]);
								return 0;
							}
						}
					}
					else {
						printf("Line %d : Command needs a operand\n", 5 * (codeline + 1));
						return 0;
					}
				}
				else if (opcode->format[0] == '3' || pst_word[1][0] == '+') { //format 3,4 must have operands(Symbol)
					if (pst_word[1][0] == '+') { //format 4 = (n,i,x,b,p,e) = (1,1,0,0,0,1)
						if ((symbol_address = returnSymbol(*newSymtab, pst_word[2])) == 0) { //operand가 symtab에 없을때
							//가능한 경우는 #숫자
							if (pst_word[2][0] == '#') {
								disp = charToDecimal(pst_word[2]);
								if (disp == -1) {
									printf("Line %d : %s is not a number\n", 5 * (codeline + 1), pst_word[2]);
									return 0;
								}
								if (disp <= 0xFFFFF && disp >= 0) {//2^20보다 클때
									obj_comnd = ((opcode->seq / 4) * 4) * 65536 * 256;
									e_flag = 1; b_flag = p_flag = 0;
									n_flag = 0;  i_flag = 1;
									flagsum = convertFlag(n_flag, i_flag, b_flag, p_flag, e_flag);
									flagsum *= 256;
									(assemcode + codeline)->object_code = obj_comnd + flagsum + disp;
								}
								else {
									printf("Line %d : address out of boundary\n", 5 * (codeline + 1));
									return 0;
								}
							}
							else {
								printf("Line %d : %s is not in SYMTAB\n", 5 * (codeline + 1), pst_word[2]);
								return 0;
							}
						}
						else { //operand가 symbol
							obj_comnd = ((opcode->seq / 4) * 4) * 65536 * 256;
							e_flag = 1; b_flag = p_flag = 0;
							addressRef(assemcode, codeline, &n_flag, &i_flag);
							flagsum = convertFlag(n_flag, i_flag, b_flag, p_flag, e_flag);
							flagsum *= 256;
							if (n_flag == 1 && i_flag == 1) disp = symbol_address; //simple addressing
							else if (n_flag == 0 && i_flag == 1) { //immediate addressing
								disp = charToDecimal(pst_word[2]); //convert Characters(숫자를 나타내는) to integer
								if (disp == -1) {
									printf("Line %d : %s is not a number\n", 5 * (codeline + 1), pst_word[2]);
									return 0;
								}
							}
							(assemcode + codeline)->object_code = obj_comnd + flagsum + disp;
							direct_num++; //symbol을 이용한 format 4 = direct addressing
						}
					}
					else if (opcode->format[0] == '3') { // format 3
						if (pst_word[3][0] != '\0') {
							if (strcmp(pst_word[3], "X") == 0) { //if format 3 has two operands, the last one must be X for index addressing
								(assemcode + codeline)->index = 1;
							}
							else {
								printf("Line %d : unproper operand %s for %s\n", 5 * (codeline + 1), pst_word[3], pst_word[1]);
								return 0;
							}
						}
						if ((symbol_address = returnSymbol(*newSymtab, pst_word[2])) == 0) { //operand가 symtab에 없을때
						//가능한 경우는 #숫자
							if (pst_word[2][0] == '#') {
								disp = charToDecimal(pst_word[2]);
								if (disp == -1) { //값이 아닐때
									printf("Line %d : %s is not in SYMTAB\n", 5 * (codeline + 1), pst_word[2]);
									return 0;
								}
								if (disp <= 0xFFF && disp >= 0) {//2^12보다 클때
									obj_comnd = ((opcode->seq / 4) * 4) * 65536;
									e_flag = 0;
									b_flag = p_flag = 0; //direct addressing for immediate addresing of a constant
									n_flag = 0;  i_flag = 1;
									flagsum = convertFlag(n_flag, i_flag, b_flag, p_flag, e_flag);
									(assemcode + codeline)->object_code = obj_comnd + flagsum + disp;
								}
								else {
									printf("Line %d : address out of boundary\n", 5 * (codeline + 1));
									return 0;
								}
							}
							else {
								if (strcmp(pst_word[1], "RSUB") == 0) { //if the opcode is RSUB
									obj_comnd = ((opcode->seq / 4) * 4) * 65536;
									e_flag = b_flag = p_flag = 0;
									n_flag = 1;  i_flag = 1;
									flagsum = convertFlag(n_flag, i_flag, b_flag, p_flag, e_flag);
									(assemcode + codeline)->object_code = obj_comnd + flagsum;
								}
								else {
									printf("Line %d : %s is not in SYMTAB\n", 5 * (codeline + 1), pst_word[2]);
									return 0;
								}
							}
						}
						else { //operand가 symbol
							pc = (assemcode + codeline + 1)->assign_address; //pc는 다음 assem line을 가리킨다.
							disp = symbol_address - pc; //calculating displacement
							obj_comnd = (((opcode->seq) / 4) * 4) * 65536; //opcode 수의 뒤 2비트를 비워주기 위해 나누었다가 곱해준다. 그리고 비트연산
							if (disp <= 2047 && disp >= -2048) { //try pc relative
								if (disp < 0) disp = 0x1000 + disp;
								b_flag = 0; p_flag = 1; e_flag = 0;
								addressRef(assemcode, codeline, &n_flag, &i_flag);
								flagsum = convertFlag(n_flag, i_flag, b_flag, p_flag, e_flag);
								(assemcode + codeline)->object_code = obj_comnd + flagsum + disp;
								if (strcmp(opcode->comnd, "LDB") == 0) base = symbol_address; //base 초기화
							}
							else { //base relative
								if (base_addressing) { //code에 base relative를 쓸 수 있다.
									disp = symbol_address - base;
									if (disp <= 4096 && disp >= 0) {
										b_flag = 1; p_flag = 0; e_flag = 0;
										addressRef(assemcode, codeline, &n_flag, &i_flag);
										flagsum = convertFlag(n_flag, i_flag, b_flag, p_flag, e_flag); //x에 대해서 고려..
										(assemcode + codeline)->object_code = obj_comnd + flagsum + disp;
									}
									else {
										printf("Line %d : Cannot reach the address\n", 5 * (codeline + 1));
										return 0;
									}
								}
								else {
									printf("Line %d : Addressing error : Base relative addressing not permitted\n", 5 * (codeline + 1));
									return 0;
								}
							}
						}
						if ((assemcode + codeline)->index == 1) {//index addressing  //check if x_flag is on!
							x_flag = 1; x_flag *= 32768;
							(assemcode + codeline)->object_code += x_flag;
						}
					}
				}
			}
		}
		codeline++;
	}
	return codeline;
}

void addressRef(Assemcode *assemcode, int codeline, int *n_flag, int *i_flag) {
	switch ((assemcode + codeline)->code[2][0]) {
	case '#':
		*n_flag = 0; //immediate addressing
		*i_flag = 1;
		break;
	case '@':
		*n_flag = 1; //indirect addressing
		*i_flag = 0;
		break;
	default:
		*n_flag = 1; //simple addressing
		*i_flag = 1;
		break;
	}
}

int convertFlag(int n_flag, int i_flag, int b_flag, int p_flag, int e_flag) {
	int result = n_flag * 131072 + i_flag * 65536 + b_flag * 16384 + p_flag * 8192 + e_flag * 4096;
	return result;
}

int charToDecimal(char *word) {
	unsigned int i, k;
	unsigned int sum = 0;
	char symbol_buf[20];
	strcpy(symbol_buf, word);
	if (word[0] == '#') {
		for (i = 1; i < strlen(word); i++) {
			symbol_buf[i - 1] = word[i];
		}
		symbol_buf[i - 1] = '\0';
	}
	for (i = strlen(symbol_buf), k = 0; i >= 1; i--, k++) {
		if (symbol_buf[i - 1] >= 48 && symbol_buf[i - 1] <= 57) {
			sum += (symbol_buf[i - 1] - 48) * expondec(k);
		}
		else { //then word is not a 숫자 character
			return -1;
		}
	}
	return sum;
}

int convertConstant(char *word) { //BYTE gets String, but  WORD gets value
	unsigned int i = 0, k = 0, res = 1;
	unsigned int sum = 0;
	char word_buf[20];
	for (i = 2; i < strlen(word) - 1; i++) {
		word_buf[i - 2] = word[i];
	}
	word_buf[i - 2] = '\0';
	if (word[0] == 'X') {//BYTE X'1byte'
		sum = converthex(word_buf);
	}
	else sum = -1;
	return sum;
}

int returnReg(char *word) {
	char word_buf[10];
	int i = 0;
	if (word[strlen(word) - 1] == ',') {
		while (word[i] != ',') {
			word_buf[i] = word[i];
			i++;
		}
		word_buf[i] = '\0';
	}
	else strcpy(word_buf, word);

	if (word_buf[0] == 'A') return 0;
	else if (word_buf[0] == 'X') return 1;
	else if (word_buf[0] == 'L')	 return 2;
	else if (word_buf[0] == 'B') return 3;
	else if (word_buf[0] == 'S') 	return 4;
	else if (word_buf[0] == 'T') 	return 5;
	else if (word_buf[0] == 'F') 	return 6;
	else 	return 10;
}

void writeListFile(Assemcode *assemcode, char *filename, int codeline) {
	FILE *list_file;
	char *word;
	char print_buf[9];
	char temp[9]; char word_buf[64];
	int i = 0; int len;
	int k;
	char file_name[20], file_exten[6] = ".lst", file_temp[30];
	strcpy(file_temp, filename);
	strcpy(file_name, strtok(file_temp, "."));
	strcat(file_name, file_exten);
	list_file = fopen(file_name, "w");
	for (; i <= codeline; i++) {
		//%4d   %4X   %s   %s   %s %s   %X
		memset(print_buf, '\0', 9);
		memset(temp, '\0', 9);
		fprintf(list_file, "%4d", (assemcode + i)->line_number);
		if ((assemcode + i)->code[0][0] == '.') { //if its a comment line
			fprintf(list_file, "             %s\n", (assemcode + i)->comment);
		}
		else {
			if ((assemcode + i)->assign_address == -1) { //END
				fprintf(list_file, "       ");
			}
			else if (strcmp((assemcode + i)->code[1], "BASE") == 0) { fprintf(list_file, "       "); }
			else {
				fprintf(list_file, "   %04X", (assemcode + i)->assign_address);
			}
			fprintf(list_file, "   %-15s   %-15s   %-10s  %-10s  ", (assemcode + i)->code[0], (assemcode + i)->code[1], (assemcode + i)->code[2], (assemcode + i)->code[3]);
			if (strcmp((assemcode + i)->code[1], "BASE") == 0) {}
			else if (strcmp((assemcode + i)->code[1], "END") == 0) {}
			else if (strcmp((assemcode + i)->code[1], "RESW") == 0) {}
			else if (strcmp((assemcode + i)->code[1], "RESB") == 0) {}
			else if (strcmp((assemcode + i)->code[1], "START") == 0) {}
			else {
				sprintf(print_buf, "%X", (assemcode + i)->object_code);
				word = (assemcode + i)->code[1];
				if ((assemcode + i)->format == '5') { //BYTE or WORD
					word = (assemcode + i)->code[2];
					memset(word_buf, '\0', 64);
					if (strcmp((assemcode + i)->code[1], "WORD") == 0) {
						valToCharWord(print_buf);
						fprintf(list_file, "%-s\n", print_buf);
						continue;
					} //X'FF1" 일때 0FF1으로 출력할 것인가?
					else { //BYTE일 때
						if (word[0] == 'X') {
							if (strlen((assemcode + i)->objcode_string) % 2 == 1) { //hexadecimal의 길이가 홀수일때 앞에 0을 넣어준다
								strcpy(word_buf, "0");
								strcat(word_buf, (assemcode + i)->objcode_string);
								fprintf(list_file, "%-s", word_buf);
								continue;
							}
						}
						fprintf(list_file, "%-s", (assemcode + i)->objcode_string);
					}
				}
				else if ((assemcode + i)->format != 0) {
					if (word[0] == '+') {//format 4
						if (strlen(print_buf) < 8) {
							len = strlen(print_buf);
							strcpy(temp, print_buf);
							for (k = 8 - len; k < 8; k++) print_buf[k] = temp[k - (8 - len)];
							for (k = 0; k < 8 - len; k++) print_buf[k] = '0';
							print_buf[8] = '\0';
						}
					}
					else {
						if ((assemcode + i)->format == '3') {//format 3
							if (strlen(print_buf) < 6) {
								len = strlen(print_buf);
								strcpy(temp, print_buf);
								for (k = 6 - len; k < 6; k++) print_buf[k] = temp[k - (6 - len)];
								for (k = 0; k < 6 - len; k++) print_buf[k] = '0';
								print_buf[6] = '\0';
							}
						}
					}
					fprintf(list_file, "%-s", print_buf);
				}
				else {
					fprintf(list_file, "%-X", (assemcode + i)->object_code); //forma 1, 2
				}
			}
			fprintf(list_file, "\n");
		}
	}
	fclose(list_file);
}
void writeObjectCode(Assemcode *assemcode, char *filename, int codeline, int locctr) {
	FILE *obj_file;
	int code_line = 0, len = 0, i, j, k = 0, l = 0, m;
	char title_buf[7], print_buf[70], obcode_buf[70], temp[7];
	char *word;
	int program_len, len_limit = 0;
	char total_text_buf[70]; char word_buf[70];
	int subrout_flag = 0; int iter_times = 0;
	char subroutine[10] = { '\0', };
	int *modify = (int *)malloc(sizeof(int)*direct_num);
	unsigned int render_obcode;
	char file_name[20], file_exten[6] = ".obj", file_temp[30];
	strcpy(file_temp, filename);
	strcpy(file_name, strtok(file_temp, "."));
	strcat(file_name, file_exten);
	obj_file = fopen(file_name, "w");

	if (strcmp(assemcode->code[1], "START") == 0) {
		word = (assemcode)->code[0];
		fprintf(obj_file, "H");
		strcpy(title_buf, (assemcode)->code[0]);
		if ((len = strlen(word)) < 6) { //when program title length is smaller than 6(full length)
			for (j = len; j < 6; j++) title_buf[j] = ' ';
			title_buf[j] = '\0';
		}
		fprintf(obj_file, "%s", title_buf); //printing program name

		strcpy(print_buf, (assemcode)->code[2]);
		valToCharWord(print_buf); //rendering program address
		fprintf(obj_file, "%s", print_buf); //printing program starting address

		memset(print_buf, '\0', 70); //initialize the print_buffer

		program_len = locctr - converthex((assemcode)->code[2]); //calculating program length
		sprintf(print_buf, "%X", program_len); //converting program length into string
		valToCharWord(print_buf); //rendering the program length string
		fprintf(obj_file, "%s\n", print_buf); //printing program length
		code_line++;
	}
	else { //if the first line is not START
		fprintf(obj_file, "H      000000");
		program_len = locctr;
		memset(print_buf, '\0', 70);
		sprintf(print_buf, "%X", program_len);
		valToCharWord(print_buf);
		fprintf(obj_file, "%s\n", print_buf);
	}


	while (strcmp((assemcode + code_line)->code[1], "END") != 0) {
		if (strcmp((assemcode + code_line)->code[1], "RESB") == 0 || strcmp((assemcode + code_line)->code[1], "RESW") == 0) { len_limit++;  code_line++; continue; }
		if (strcmp((assemcode + code_line)->code[1], "BASE") == 0) { code_line++; len_limit++; continue; }
		if ((assemcode + code_line)->code[0][0] == '.') {
			code_line++; len_limit++; continue;
		}
		memset(total_text_buf, '\0', 70); //initialize the text record_buffer
		memset(obcode_buf, '\0', 70); //initialize the obcode_buffer
		len = 0; iter_times = 0;
		strcpy(total_text_buf, "T");

		i = code_line = len_limit; //store the starting code line into variable i and code_line
		sprintf(print_buf, "%X", (assemcode + code_line)->assign_address); //starting address
		valToCharWord(print_buf); //rendering the starting address of text record String
		strcat(total_text_buf, print_buf); //concat the starting address after 'T'

		while (len < 60) {
			iter_times++;
			memset(obcode_buf, '\0', 70);
			memset(word_buf, '\0', 70);
			if ((assemcode + code_line)->code[0][0] == '.') { code_line++; continue; }
			if (strcmp((assemcode + code_line)->code[1], "BASE") == 0) { code_line++; continue; }
			if (strcmp((assemcode + code_line)->code[1], "RESB") == 0 || strcmp((assemcode + code_line)->code[1], "RESW") == 0) { code_line++; continue; }
			if (strcmp((assemcode + code_line)->code[1], "BASE") == 0) { code_line++;	continue; }
			if (strcmp((assemcode + code_line)->code[1], "END") == 0) { code_line++;  break; }
			if ((assemcode + code_line)->code[1][0] == '+') {
				if (strcmp((assemcode + code_line)->code[1], "+JSUB") == 0) { //getting Subroutine label
					modify[l++] = code_line; //for recording the code line of modification
					if (subrout_flag == 0) {
						strcpy(subroutine, (assemcode + code_line)->code[2]);
						subrout_flag++;
					}
				}
			}
			if (strcmp((assemcode + code_line)->code[1], "JSUB") == 0) { //getting Subroutine label
				if (subrout_flag == 0) {
					strcpy(subroutine, (assemcode + code_line)->code[2]);
					subrout_flag++;
				}
			}
			if (subrout_flag == 1) {
				if (strcmp(subroutine, (assemcode + code_line)->code[0]) == 0 && iter_times != 1) { //if Subroutine label is found
					subrout_flag++;
					code_line++;
					break;
				}
			}
			sprintf(obcode_buf, "%X", (assemcode + code_line)->object_code);//converting object_code into string
			if ((assemcode + code_line)->format == '3') valToCharWord(obcode_buf); //rendering the object code string
			else if ((assemcode + code_line)->format == '5') { //if it is byte or word
				word = (assemcode + code_line)->code[2];
				if (strcmp((assemcode + code_line)->code[1], "WORD") == 0) {
					valToCharWord(obcode_buf);
				}
				else {
					memset(obcode_buf, '\0', 70);
					if (word[0] == 'X') {
						if (strlen((assemcode + code_line)->objcode_string) % 2 == 1) { //hexadecimal의 길이가 홀수일때 앞에 0을 넣어준다
							strcpy(obcode_buf, "0");
							strcat(obcode_buf, (assemcode + code_line)->objcode_string);
						}
						else 	strcpy(obcode_buf, (assemcode + code_line)->objcode_string);
					}
					else strcpy(obcode_buf, (assemcode + code_line)->objcode_string);
				}
			}
			len += strlen(obcode_buf);
			code_line++;
		}

		if (iter_times != 1) {
			len -= strlen(obcode_buf);
			len_limit = --code_line; //a length for text record
		}
		else len_limit = code_line;

		memset(print_buf, '\0', 70); //initialize the print_buffer
		sprintf(print_buf, "%X", len / 2); //converting len_limit into String
		if ((len = strlen(print_buf)) < 2) {//need '0' in the first index, rendering the length index
			print_buf[1] = print_buf[0];
			print_buf[0] = '0';
			print_buf[2] = '\0';
		}
		strcat(total_text_buf, print_buf); //concat the length of text record after the starting address

		for (; i < len_limit; i++) {
			if (strcmp((assemcode + i)->code[1], "START") == 0) continue; //ignoring first line
			if (strcmp((assemcode + i)->code[1], "RESB") == 0 || strcmp((assemcode + i)->code[1], "RESW") == 0) continue;
			if (strcmp((assemcode + i)->code[1], "BASE") == 0) continue;
			if ((assemcode + i)->code[0][0] == '.') continue;
			memset(obcode_buf, '\0', 70); //initialize the print_buffer
			sprintf(obcode_buf, "%X", (assemcode + i)->object_code); //converting object code into String
			if ((assemcode + i)->format == '3') valToCharWord(obcode_buf);
			else if ((assemcode + i)->format == '5') { //if it is byte or word
				word = (assemcode + i)->code[2];
				if (strcmp((assemcode + i)->code[1], "WORD") == 0) {
					valToCharWord(obcode_buf);
				}
				else {
					memset(obcode_buf, '\0', 70);
					if (word[0] == 'X') {
						if (strlen((assemcode + i)->objcode_string) % 2 == 1) { //hexadecimal의 길이가 홀수일때 앞에 0을 넣어준다
							strcpy(obcode_buf, "0");
							strcat(obcode_buf, (assemcode + i)->objcode_string);
						}
						else 	strcpy(obcode_buf, (assemcode + i)->objcode_string);
					}
					else strcpy(obcode_buf, (assemcode + i)->objcode_string);
				}
			}
			strcat(total_text_buf, obcode_buf);
		}
		fprintf(obj_file, "%s\n", total_text_buf);
	}

	//Modification record
	if (direct_num != 0) {
		for (l = 0; l < direct_num; l++) {
			memset(total_text_buf, '\0', 70);
			strcpy(total_text_buf, "M");
			memset(print_buf, '\0', 70);
			sprintf(print_buf, "%X", ((assemcode + modify[l])->assign_address) + 1); //converting assign_address + 1, where the address field begins, into string
			valToCharWord(print_buf); //rendering address
			strcat(total_text_buf, print_buf);
			memset(print_buf, '\0', 70);
			render_obcode = ((assemcode + modify[l])->object_code * (16 * 16 * 16)) / (16 * 16 * 16);
			sprintf(print_buf, "%X", render_obcode);
			if ((assemcode + modify[l])->code[1][0] == '+') m = 5;
			else {
				if ((assemcode + modify[l])->format == '3') m = 3; //format에 따라 address field의 길이가 달라짐
			}
			if ((len = strlen(print_buf)) < m) {//starting address rendering
				strcpy(temp, print_buf);
				for (k = m - len; k < m; k++) print_buf[k] = temp[k - (m - len)];
				for (k = 0; k < m - len; k++) print_buf[k] = '0';
				print_buf[m] = '\0';
			}
			len = strlen(print_buf);
			sprintf(print_buf, "0%X", len);
			strcat(total_text_buf, print_buf);
			fprintf(obj_file, "%s\n", total_text_buf);
		}
	}
	memset(print_buf, '\0', 70); //initialize the print_buffer
	if (strcmp(assemcode->code[1], "START") == 0) {
		strcpy(print_buf, (assemcode)->code[2]);
		valToCharWord(print_buf); //rendering program address
		fprintf(obj_file, "E%s\n", print_buf);//printing the program executable address
	}
	else {
		fprintf(obj_file, "E000000\n");
	}

	strcpy(file_temp, filename);
	memset(file_name, '\0', 20);
	strcpy(file_name, strtok(file_temp, "."));
	printf("output file : [%s.lst], [%s.obj]\n", file_name, file_name);
	free(modify);
	fclose(obj_file);

}

void valToCharWord(char *print_buf) {
	int len, k;
	char temp[7];
	if ((len = strlen(print_buf)) < 6) {//starting address rendering
		strcpy(temp, print_buf);
		for (k = 6 - len; k < 6; k++) print_buf[k] = temp[k - (6 - len)];
		for (k = 0; k < 6 - len; k++) print_buf[k] = '0';
		print_buf[6] = '\0';
	}
}

int type(char *filename) {
	FILE *fp = fopen(filename, "r");
	char strTemp[255];
	if (fp != NULL) {
		while (!feof(fp))
		{
			fgets(strTemp, sizeof(strTemp), fp);
			if (feof(fp)) break;
			printf("%s", strTemp);
		}
	}
	else {
		printf("No file as %s in directory\n", filename);
		return 0;
	}
	fclose(fp);
	return 1;
}
