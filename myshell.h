#include "get_path.h"

int pid;
void sig_handler(int sig);
int sigignore(int sig);
int myshell( int argc, char **argv, char **envp);
char *which(char *command, struct pathelement *pathlist);
char *where(char *command, struct pathelement *pathlist);
void list(char *dir);
void printenv(char **envp);
char **stringarray(char *str, char **args, int *argCount);
void freepathlist(struct pathelement *pathlist);

#define BUFFER 128
#define PROMPTMAX 64
#define MAXARGS 16
//#define MAXLINE 128
