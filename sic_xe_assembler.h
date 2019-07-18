#ifndef SPPROJ1_H
#define SPPROJ1_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_SIZE_COMND 50

typedef struct c_node {
        char comnd[MAX_SIZE_COMND];
        unsigned int seq; //refers sequence number in LinkedList or opcode number in hashtable
        char format[8];
	struct c_node *next;
}Node;

typedef struct _List {
        Node * first;
        Node * cur;
}LinkedList;

typedef struct _hash {
        char data;
        Node * next;
}Hash;

typedef struct _assem {
	char code[4][64];
	int word_num;
	unsigned int assign_address;
	unsigned int object_code;
	char objcode_string[64];
	char *comment;
	int line_number;
	char index;
	char format;
}Assemcode;

int checkright(char* inp);
int balancer(int comndnum, char *arg[], int argnum, Assemcode *assemcode, LinkedList *hist, Hash * optable, Hash **symtable, Hash **newSymtab, unsigned char *mem);
void help(void);
void dir(void);
void history(LinkedList *hist);
void addLlist(LinkedList *hist, Hash *table, char **inpcomnd, int mode);
void dump(char *arg[], int argnum, unsigned char *mem);
void dumpprint(int *range, unsigned char*mem);
int exponhex(int x);
int converthex(char *arg);
int edit(char *arg[], unsigned char *mem); //¼öÁ¤7
int fill(char *arg[], unsigned char *mem);
void reset(unsigned char *mem);
int opcodesearch(char *arg[], Hash *table, Node **opcode, int mode);
int ophashMake(Hash *table);
int ophashindex(char *opname);
int opcodelist(Hash *table);
int letterfilter(char *inpcmnd, int i, int *blankflag, int *breakflag);
void freemalloc(Assemcode *assemcode, LinkedList *hist, Hash * optable, Hash *symtab, unsigned char *mem);
int expondec(int x);
int assemPassOne(char filename[], Assemcode *assemcode, Hash **newSymtab, Hash *optable);
int formatCal(char word_opcode[], Node * opcode, int Locctr);
int checkSymbol(Hash *symtab, char *word, int Locctr);
int hashSymbol(char *word);
int assemLetterFilter(char *inpcmnd, int index, int *blankflag, int *breakflag);
int assemPassTwo(Assemcode *assemcode, Hash **newSymtab, Hash * optable);
void symtabPrint(Hash *symtab);
void addressRef(Assemcode *assemcode, int codeline, int *n_flag, int *i_flag);
int convertFlag(int n_flag, int i_flag, int b_flag, int p_flag, int e_flag);
int charToDecimal(char *word);
int convertConstant(char *word);
int returnReg(char *word);
void writeListFile(Assemcode *assemcode, char *filename, int codeline);
void writeObjectCode(Assemcode *assemcode, char *filename, int codeline, int locctr);
void valToCharWord(char *print_buf);
int type(char *filename);


extern int lastaddress;
extern int opflag;
extern int symflag;
extern int base_addressing;
extern int subrout_num; //recording subroutin numbers for new line for text record, initialzied in pass one
extern int direct_num; //recording direct addressing number for modification record, initialized in pass two

#endif
