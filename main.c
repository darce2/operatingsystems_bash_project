/*******************************************************************************
/
/      filename:  main.c
/
/   description:  implementation of the Linux wc command in C
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

#include "func.h"

main(int argc, char *argv[]){
   char *cmd = malloc(sizeof(*cmd)*(MAX_CANON+1));
   char **args;
   FILE* fileP;
   int status = 1, duping = -1, i;
   if(argc == 2) fileP = fopen(argv[1],"r");
   else fileP = stdin;
   fprintf(stderr, "%s", PROMPT_STRING);
   while(fgets(cmd,MAX_CANON,fileP)!=NULL){
      argCount = 0;
      args = parser(cmd);
      i = process(args);//run the process
      fprintf(stderr, "%s", PROMPT_STRING);
   }//end status while
   free(args);
   free(cmd);
return 1;
}//END main
