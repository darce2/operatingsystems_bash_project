/*******************************************************************************
/
/      filename:  func.h
/
/   description:  functions for implementation of the Linux wc command in C
/
/        author:  D'Arcy, Arlen	
/      login id:  FA_15_CPS356_21
/
/         class:  CPS 356
/    instructor:  Perugini
/    assignment:  Homework #2
/
/      assigned:  September 10, 2015
/           due:  September 29, 2015
/
******************************************************************************/
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#define PROMPT_STRING "You are correct, sir!>"
#define DELIMS " \t\n"
#ifndef MAX_CANON
#define MAX_CANON 8192
#endif
//globals
int argCount;
int endin, endout, endapend;
const char *builtin[] = {"cd", "envvars", "print", "pause", "quit"};
extern char** environ;
int duper=-1, doubleD = 0;
int copy_stdin;
int copy_stdout;
//Set up Prototypes
char **parser(char *cmd);
int process(char **args);
int redirect(char **args);
int redup(void);
int exe(char **args);
int print(char **args);
int quit(char **args);
int envar(char **args);
int pause(void);
int cd(char **args);

char **parser(char *cmd){ //parse cmdline input
   char **arguments = malloc((strlen(cmd) * sizeof(char*))+4);
   char *parsed;
   int pos = 0;
   parsed = strtok(cmd, DELIMS);
   while(parsed != NULL){
      arguments[pos] = parsed;//argument vector
      parsed = strtok(NULL, DELIMS);
      pos++;//posistion to place 
      argCount++;//for print state
   }
   arguments[pos+1] = NULL;
   return arguments;
}//end parser funct
int redirect(char **args){//function determins correct duping
   int i;
   FILE* fileI;
   FILE* fileO;
   for(i=0;i<argCount;i++){
      if(strcmp(args[i],"<")==0){//dup stdin to file
         duper = 0;
         doubleD += 1;
         copy_stdin = dup(0);
         endin = i;
         if((fileI = fopen(args[i+1], "r"))==NULL){
            fprintf (stderr,"Failed to open file\n");
            break;}
         if(dup2(fileno(fileI), fileno(stdin))==-1){
            fprintf (stderr,"Failed to redirect stdin\n");
            return 1;}
         if(fclose(fileI) == EOF) {
            fprintf (stderr,"Failed to close the file\n");
            return 1;}
      }//end input redir
      if(strcmp(args[i],">")==0){//dup stdout to file out
         duper = 1;
         endout = i;
         doubleD += 1;
         copy_stdout = dup(1);
         if((fileO = fopen(args[i+1], "w"))==NULL){
            fprintf (stderr,"Failed to write to file\n");
            return 1;}
         if(dup2(fileno(fileO), fileno(stdout))==-1){
            fprintf(stderr,"Failed to redirect stdout.\n");
            return 1;}
         if(fclose(fileO) == EOF) {
             fprintf(stderr, "Failed to close the file.\n");
             return 1;}
      }//end write redir
      if(strcmp(args[i],">>")==0){//append file
         endout = i;
         duper = 1;
         doubleD += 1;
         copy_stdout = dup(1);
         if((fileO = fopen(args[i+1], "a"))==NULL){
            fprintf (stderr,"Failed to append file\n");
            return 1;}
         if(dup2(fileno(fileO), fileno(stdout))==-1){
            fprintf(stderr,"Failed to redirect stdout.\n");
            return 1;}
         if(fclose(fileO) == EOF) {
            fprintf(stderr, "Failed to close the file.");
            return 1;}
      }//end append redir
   }//end for args walkthrou
   return 1;
}//end redirect
int redup(void){//reset file pointers and flush
   fflush(stdin);
   fflush(stdout);
   if(dup2(copy_stdin, 0)==-1){
      fprintf(stderr,"Failed to redirect back to stdin.\n");
      return 1;}
   if(dup2(copy_stdout, 1)==-1){
      fprintf(stderr,"Failed to redirect back to stdout.\n");
      return 1;}
   doubleD = 0;
   duper = -1;
return 1;
}//end redup
int process(char **args){//process all commands
   int i, func, k=0;
   if(args[0] == NULL) return 1;
   for(i=0;i<5;i++){
      if(strcmp(builtin[i],args[0])==0){ 
         k=1;
         break;}
   }
   // check for rediection
   func = redirect(args);
   if(k==1){ //Which Builtin to run depends on comparison
      if(i==0) func = cd(args);
      else if(i==1) func = envar(args);
      else if(i==2) func = print(args);
      else if(i==3) func = pause();
      else if(i==4) func = quit(args);}
   else exe(args);//Not a builtin so exe that ish
   if (duper != -1) func = redup();

return 1;
}// end process commands function
int exe(char **args){
   pid_t child_pid, ppid, waitforprocess;
   int stat;
   child_pid = fork();
   if (child_pid == -1) {
      perror("Failed to fork... EXITING...");
      return 0;}
   else if(child_pid == 0){ //child process
      if(duper == 0 && doubleD < 2){//if stdin duped, send in only up to the < char
         char **newArg = malloc(MAX_CANON * sizeof(char*));
         int count = endin+1;
         int a = 0;
         for(count;args[count]!=NULL;count++) newArg[a++] = args[count];
         execvp(args[0],newArg);
         perror("Failed to run command\n");
         exit(1);}
      if(duper == 1 && doubleD < 2){//if stdout duped, send in only up to the <> char
         char **newArg = malloc(MAX_CANON * sizeof(char*));
         int a = 0;
         for(a;a<endout;a++) newArg[a] = args[a];
         execvp(args[0],newArg);
         perror("Failed to run command\n");
         exit(1);}
      if(doubleD == 2){//if doubly duped
         char **newArg = malloc(MAX_CANON * sizeof(char*));
         int x=0,a = endin+1;
         for(a;a<endout;a++) newArg[x++] = args[a];
         execvp(args[0],newArg);
         perror("Failed to run command\n");
         exit(1);}
      else execvp(args[0], args);
      fprintf(stderr,"-mysh:'%s': command not found\n", args[0]);
      exit (1);
   }//end child processes
   else wait(&stat); // parent process
return 1;
}// END EXEC function
int cd(char **args){//function to move around directory
   char* directory = malloc(MAX_CANON * sizeof(char*));
   int err;
   getcwd(directory,MAX_CANON);
   err = chdir(args[1]); 
   if (err < 0 && (args[1]!=NULL) && (duper == -1)) 
      fprintf(stderr, "-myshell: %s: %s: No such file or directory\n", args[0], args[1]);
   else if (err < 0) printf("%s\n", directory);
return 1;
}//end CD function
int envar(char **args){//Print Environment Variables
   int count;
    if (environ != NULL)
      for (count=0; environ[count] != NULL; count++)
        printf ("environ[%d]: %s\n", count, environ[count]);
return 1;
}//end enironment function
int pause(void){
   getpass("Paused until return...");
return 1;
}//end pause function
int print(char **args){
   int i;
   if(duper == -1){
      for(i=1;i<argCount;i++) printf("%s ", args[i]);
      printf("\n");}
return 1;
}//End print
int quit(char **args){
exit (0);
}//quit program