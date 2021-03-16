# choose your compiler
CC=gcc
#CC=gcc -Wall

myshell: myshell.o get_path.o 
	$(CC) -g myshell.o get_path.o -o myshell
#	$(CC) -g myshell.o get_path.o bash_getcwd.o -o myshell

myshell.o: myshell.c myshell.h
	$(CC) -g -c myshell.c

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

clean:
	rm -rf myshell.o get_path.o myshell

