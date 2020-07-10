#
# CMSC257 - Utility Library 
# Code for the CMSC257 Projects
#

# Make environment
my257sh: shellex.c csapp.h csapp.c 
	gcc -pthread -Wall -g -I. $^ -o $@

clean:
	rm -f *.o my257sh 
