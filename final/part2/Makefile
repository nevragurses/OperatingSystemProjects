all : sortArrays

sortArrays: sortArrays.o
	gcc -o sortArrays sortArrays.o -lm -pthread 

sortArrays.o : sortArrays.c
	gcc -c sortArrays.c 

clean:
	rm -f sortArrays *.o
