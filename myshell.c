#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <wordexp.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "myshell.h"

void sig_handler(int sig)
{
  while (waitpid((pid_t) (-1), 0, WNOHANG) > 0)
{}
}


int main(int argc, char **argv, char **envp ) {

  //Signal Handling
  sigignore(SIGINT);
  sigignore(SIGTERM);
  sigignore(SIGTSTP);
  signal(SIGCHLD, sig_handler);


  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *cmdline = calloc(MAX_CANON, sizeof(char));  
  char *command; 
  char *arg; 
  char *pwd;
  char *owd;
  char **args;
  int user, i;
  struct pathelement *pathlist;
  struct passwd  *entry;
  char *homedir;
  user = getuid();
  entry = getpwuid(user);
  homedir = entry->pw_dir;

  if ((pwd = getcwd(NULL, PATH_MAX+1)) == NULL)
  {
     perror("getcwd");
     exit(2);
  }	
  

  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  prompt[0] = ' '; 
  prompt[1] = '\0';
  
  int argsct = 0;
  int start = 1;
  pathlist = get_path();
  while (start)
    {
      //Begin prompt
      printf("%s[%s]> ", prompt, pwd);

      arg = fgets(cmdline, BUFFER, stdin);
      int len = strlen(arg);
      arg[len-1]='\0';
      argsct=0;
      args=stringarray(arg, argv, &argsct);
      if(argsct>0){
		
	//EXIT
	if(strcmp(args[0], "exit")==0){
	  printf("Executing EXIT:\n");
	  start = 0;
	  free(args[0]);
	  free(args);
	  free(owd);
	  free(pwd);
	}
	//WHICH
	else if(strcmp(args[0], "which")==0){
	  if(argsct==1){
	    printf("Too few arguments for WHICH!\n");
	    free(args[0]);
	    free(args);
	  }
	  else{
	    printf("Executing WHICH:\n");
	    int temp = argsct;
	    while(temp > 1){
	      char *command = which(args[temp-1], pathlist);
	      printf("%s",command);
	      free(command);
	      free(args[temp-1]);
	      temp--;
	    }
	    free(args[0]);
	    free(args);
	  }
	}
	//WHERE
	else if(strcmp(args[0], "where")==0){
	  if(argsct==1){
	    printf("Not enough arguments for WHERE!\n");
	    free(args[0]);
	    free(args);
	  }
	  else {
	    printf("Executing WHERE:\n");
	    int temp = argsct;
	    while(temp > 1){
	      char *command = which(args[temp-1], pathlist);
	      printf("%s",command);
	      free(command);
	      free(args[temp-1]);
	      temp--;
	    }
	    free(args[0]);
	    free(args);
	  }
	}
	//CD
	else if(strcmp(args[0], "cd")==0){
	  if(argsct > 2){
	    printf("Too many arguments for CD!\n");
	  }
	  else if(argsct==1) {
	    if(chdir(homedir) == 0){
	      printf("Executing CD:\n");
	      chdir(homedir);
	      free(owd);
	      owd = pwd;
	      pwd = getcwd(NULL, PATH_MAX+1);
	    }
	    else{
	      printf("Invalid directory!\n");
	    }
	  }
	  else if(strcmp(args[1], "-")==0) {
	    if(chdir(owd)==0){
	      printf("Executing CD:\n");
	      chdir(owd);
	      free(owd);
	      owd = pwd;
	      pwd = getcwd(NULL, PATH_MAX+1);
	    }
	    else {
	      printf("Invalid directory!\n");
	    }
	  }
	  else{
	    if(chdir(args[1])==0){
	      printf("Executing CD:\n");
	      chdir(args[1]);
	      free(owd);
	      owd=pwd;
	      pwd=getcwd(NULL, PATH_MAX+1);
	    }
	    else{
	      printf("Invalid directory!\n");
	    }
	  }
	  int temp = argsct;
	  while(temp>0){
	    free(args[temp-1]);
	    temp--;
	  }
	  free(args);
	}
	//PWD
	else if(strcmp(args[0], "pwd")==0){
	  printf("Executing PWD:\n");
	  printf("[%s]\n", pwd);
	  free(args[0]);
	  free(args);
	}
	//LIST
	else if(strcmp(args[0], "list")==0){
	  printf("Executing LIST:\n");
	  if(argsct==1){
	    list(pwd);
	    free(args[0]);
	    free(args);
	  }
	  else{
	    int temp = argsct;
	    while(temp >1){
	      list(args[temp-1]);
	      free(args[temp-1]);
	      temp--;
	    }
	    free(args[0]);
	    free(args);
	  }
	}
	//PID
	else if(strcmp(args[0], "pid")==0){
	  printf("Executing PID:\n");
	  printf("PID: %d\n", getpid());
	  free(args[0]);
	  free(args);
	}
	//KILL
	else if(strcmp(args[0], "kill")==0){
	  if(argsct==1){
	    printf("KILL must have at least one argument!\n");
	    free(args[0]);
	    free(args);
	  }
	  else if(argsct > 3){
	    printf("Too many arguments for KILL!\n");
	    int temp = argsct;
	    while(temp > 0){
	      free(args[temp-1]);
	      temp--;
	    }
	    free(args);
	  }
	  else if(argsct == 2){
	    printf("Executing KILL:\n");
	    pid_t apid = atoi(args[1]);
	    kill(apid, SIGTERM);
	    free(args[1]);
	    free(args[0]);
	    free(args);
	  }
	  else{
	    printf("Executing KILL:\n");
	    pid_t apid = atoi(args[2]);
	    int sig = -1 * atoi(args[1]);
	    printf("%d\n", sig);
	    kill(apid, sig);
	    free(args[2]);
	    free(args[1]);
	    free(args[0]);
	    free(args);
	  }
	}
	
	//PROMPT
	else if(strcmp(args[0], "prompt")==0){
	  if(argsct==1){
	    printf("Executing PROMPT:\n"); 
	    printf("input PROMPT prefix: ");
	    fgets(prompt, BUFFER, stdin);
	    int plen = strlen(prompt);
	    if(prompt[plen-1]=='\n')
	      prompt[plen-1]='\0';
	  }
	  else if(argsct > 2) {
	    printf("Too many arguments for PROMPT\n");
	    int i = argsct;
	    while(i>1){
	      free(args[i-1]);
	      i--;
	    }
	  }
	  else{
	    printf("Executing PROMPT:\n"); 
	    strcpy(prompt, args[1]);
	    free(args[1]);
	  }
	  free(args[0]);
	  free(args);
	}
	//PRINTENV
	else if(strcmp(args[0], "printenv")==0){
	  if(argsct==1){
	    printf("Executing PRINTENV:\n");
	    int j;
	    for(j = 0; envp[j] != NULL; j++){
	      printf("%s\n", envp[j]);
	    }
	  }
	  else if(argsct==2){
	    if(getenv(args[1])){
	      printf("Executing PRINTENV:\n");
	      char *p = getenv(args[1]);
	      printf("%s\n", p);
	    }
	    else{
	      printf("env not found!\n");
	    }
	  }
	  else{
	    printf("Too many arguments for PRINTENV!\n");
	  }
	  int temp = argsct;
	  while(temp>0){
	    free(args[temp-1]);
	    temp--;
	  }
	  free(args);
	}
	//SETENV
	else if(strcmp(args[0], "setenv")==0){
	  if(argsct==1){
	    printf("Executing SETENV:\n");
	    int k;
	    for(k = 0; envp[k] != NULL; k++){
	      printf("%s\n", envp[k]);
	    }
	    
	  }
	  else if(argsct==2){
	    printf("Executing SETENV:\n");
	    setenv(args[1],"/",1);
	    if(strcmp(args[1], "HOME")==0){
	      homedir=getenv("HOME");
	    }
	    else if(strcmp(args[1],"PATH")==0){
	      free(pathlist->element);
	      freepathlist(pathlist);
	      pathlist = get_path();
	    }
	  }
	  else if(argsct==3){
	    printf("Executing SETENV:\n");
	    setenv(args[1],args[2],1);
	    if(strcmp(args[1], "HOME")==0){
	      homedir=getenv("HOME");
	    }
	    else if(strcmp(args[1], "PATH")==0){
	      freepathlist(pathlist);
	      pathlist = get_path();
	    }
	  }
	  else{
	    printf("Too many arguments for SETENV!\n");
	  }
	  int temp = argsct;
	  while(temp>0){
	    free(args[temp-1]);
	    temp--;
	  }
	  free(args);
	}

	else if(strchr(arg, '*') !=NULL || strchr(arg, '?') !=NULL){
	  wordexp_t p;
	  char **w;
	  int position;
	  wordexp(arg, &p, 0);
	  w = p.we_wordv;
	  for(position = argsct; position < p.we_wordc; position++){
	    printf("%s\n", w[position]);
	  }
	  wordfree(&p);
	  int temp = argsct;
	  while(temp>0){
	    free(args[temp-1]);
	    temp--;
	  }
	  free(args);
	}
	
	//else execute
	else
	  {
	    char **newArgs = malloc((argsct+1) * (sizeof(char*)));
	    int j = 0;
	    while(j <=argsct){
	      if(j==argsct){
		newArgs[j] = '\0';
	      }
	      else {
		newArgs[j]=args[j];
	      }
	      j++;
	    }
	    if(args[0][0]=='/' || args[0][0] == '.'){
	      char *cmd;
	      cmd = (char *) malloc((strlen(args[0])+1)*sizeof(char));
	      strcpy(cmd, args[0]);
	      pid_t apid;
	      pid = fork();
	      if(pid == 0){
		execve(args[0], newArgs, envp);
	      }
	      else if(pid != 0){
		waitpid(pid, NULL, 0);
	      }
	      else{
		printf("command %s not found!\n", cmd);
	      }
	      free(cmd);
	    }
	    else{
	      char *abs = which(args[0], pathlist);
	      int len = strlen(abs);
	      abs[len-1]= '\0';
	      
	      if(abs == NULL){
		printf("command %s not found!\n", args[0]);
	      }
	      else if(access(abs, X_OK) == 0){
		printf("Executing %s:\n",args[0]);
		pid_t apid = fork();
		if(apid ==0){ //child
		  execve(abs, newArgs, envp);
		}
		else{//parent
		  waitpid(apid,NULL,0);
		}
	      }
	      else{
		printf("cannot acccess file\n");
	      }
	    
	      free(abs);
	      //free(newArgs);
	    }
	    int temp = argsct;
	    while(temp > 0){
	      free(args[temp-1]);
	      temp--;
	    }
	    free(newArgs);
	    free(args);
	  }
      }
      //user hits enter
      else{ 
	free(args);
      }
    }
  free(pathlist->element);
  freepathlist(pathlist);
  free(cmdline);
  free(prompt);
  return 0;
}

//Helper function for which cmd
char *which(char *command, struct pathelement *pathlist )
{
  //Loop through pathlist
  char *findcmd = malloc(200*sizeof(char));
  int cmdFound = 0; //0 if not found, 1 if found
  while(pathlist){
    sprintf(findcmd, "%s/%s", pathlist->element, command);
    if(access(findcmd, X_OK)==0){ //found
      cmdFound=1;
      sprintf(findcmd, "%s/%s\n", pathlist->element, command);
      break;
    }
    pathlist=pathlist->next;
  }
  if(cmdFound==0)
    sprintf(findcmd, "%s: command not found\n", command);
  return findcmd;
  
}

//Helper funciton for where cmd
char *where(char *command, struct pathelement *pathlist) {
  //loop through to find the locats of the command

  char* findcmd = malloc(200*sizeof(char));
  char* test = malloc(600*sizeof(char));
  int cmdFound = 0; //0 if not found, 1 if found
  int builtin = 0;

  if (strcmp(command, "exit")==0 ||
     strcmp(command,"which")==0 ||
     strcmp(command, "where")==0 ||
     strcmp(command, "cd")==0 ||
     strcmp(command, "pwd")==0 ||
     strcmp(command, "list")==0 ||
     strcmp(command, "pid")==0 ||
     strcmp(command, "kill")==0 ||
     strcmp(command, "prompt")==0 ||
     strcmp(command, "printenv")==0 ||
     strcmp(command, "setenv")==0) {
    builtin=1;
    sprintf(findcmd, "command %s\n", command);
  }

  while(pathlist){
    sprintf(test, "%s/%s", pathlist->element, command);
    if(access(test, X_OK)==0){
      char *add = malloc(200*sizeof(char));
      sprintf(add, "%s%s\n", findcmd, test);
      sprintf(findcmd, "%s", add);
      cmdFound = 1;
      free(add);
    }
    pathlist = pathlist->next;
  }
  free(test);
  if(builtin==0 && cmdFound == 0){
    sprintf(findcmd, "command %s not found!\n", command);
    return findcmd;
  }
  return findcmd;
	
}


//Helper function for list command
void list ( char *dir ) {
  DIR *adir = opendir(dir);
  struct dirent *file;

  if(adir != NULL){
    file=readdir(adir);
    printf("%s:\n", dir);
    while(file){
      printf("\t%s\n",file->d_name);
      file=readdir(adir);
    }
  }
  else{
    printf("list: cannot access %s: no such file or directory\n", dir);
  }
  free(adir);
}

//Helper function to turn a string to an array
char **stringarray(char *s, char **args, int *argCount) {
  char buffer[BUFFER];
  char delim[2] = " ";
  strcpy(buffer, s);
  char *token = strtok(buffer, " ");

  /*determine length of s*/
  int count = 0;
  while(strtok(NULL, delim)){
    count++;
  }
  args = malloc((count+1) * sizeof(char*));
  args[count]=0;

  strcpy(buffer, s);
  count = 0;
  token = strtok(buffer, delim);
  while(token != NULL){
    args[count] = (char*)malloc((strlen(token)+1) * sizeof(char*));
    strcpy(args[count],token);
    count++;
    *argCount= count;
    token = strtok(NULL, delim);
  }
  return args;
}

//Helper function to free the pathlist
void freepathlist(struct pathelement *pathlist){
  struct pathelement *temp;
  while(pathlist){
    temp = pathlist;
    pathlist = pathlist->next;
    free(temp);
  }
  free(pathlist);
}