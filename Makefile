sic_xe_assembler.out : sic_xe_assembler.o functions.o
	gcc -o sic_xe_assembler.out sic_xe_assembler.o functions.o

sic_xe_assembler.o : sic_xe_assembler.c
	gcc -c -o sic_xe_assembler.o sic_xe_assembler.c

functions.o : functions.c
	gcc -c -o functions.o functions.c
clean :
	rm *.o sic_xe_assembler.out
 
