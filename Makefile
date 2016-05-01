#******************************************************************************/#
#      file ame:  makefile
#
#   description:  makefile for project 1 shell
#
#        author:  D'Arcy, Arlen	
#      login id:  FA_15_CPS356_21
#      
#         class:  CPS 356
#    instructor:  Perugini
#    assignment:  project 1
#
#      assigned:  September 10, 2015
#           due:  September 29, 2015
#
#******************************************************************************/

all: a.out

a.out: main.o
	gcc main.o

main.o: main.c func.h
	gcc -c main.c func.h

clean:
	rm *.o a.out 

