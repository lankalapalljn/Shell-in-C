////////////////////////////////////////////////////////////////////////////////
////
////  File           : shellex.c
////  Description    : This is the main source code for for the shell program and executes the shell.
////                   
////
////   Author        : Jayanth Lankalapalli 
////   Last Modified : 4/24/20
////
//
//// Include Files
/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
void sigintHandler(int sig_num); 

int main(int argc, char* argIN[]) 
{   char *prompt = "my257sh";
    char cmdline[MAXLINE]; /* Command line */
	


    while (1) { // the main function is a loop that loops forever until terminated even after each command entered by the user
	signal(SIGINT, sigintHandler); // signal handler to prevent Ctrl C from working
	/* Read */
	if((argc>=2) && (!strcmp(argIN[1], "-p"))){ // if statement to check if the user enters a prompt for the shell name to appear
		printf("%s>", argIN[2]);
	}
	else{ // if no prompt was entered in command line then there is a default setting fo my257sh
	printf("%s>", prompt);}
                   
	Fgets(cmdline, MAXLINE, stdin); // gets the command entered in by the user
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
	eval(cmdline); // sends command to eval 


    
    } 
}

void sigintHandler(int signum){ // this method works to interrupt CTRL C from working when entered by the user
	signal(SIGINT, sigintHandler);

	fflush(stdout);
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */

    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { 
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            if (execvp(argv[0], argv) < 0) { //execvp is used to ignore envormental issues and allows the usage of nonbuilt in commands such as ls
                printf("%s: Command not found.\n", argv[0]); // if no built in nor non built in comand is used then print command not found cuz nothing was executed
                exit(1);
	
            }
        }

	/* Parent waits for foreground job to terminate */
	if (!bg) {
	    int status;
	    if (waitpid(pid, &status, 0) < 0)
		unix_error("waitfg: waitpid error");
		int exitStatus = WEXITSTATUS(status);
		printf("Child exit status: %d\n",exitStatus); // prints the childs status every time a non builtin command was used such as ls, prints 1 or 0
	}
	else
	    printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
	char str[1028];
	int a;
    if (!strcmp(argv[0], "exit")){ /* quit command */ // if user enters quit then SIGKILL is raised and the program is killed
	raise(SIGKILL);  }
    if (!strcmp(argv[0], "&")){    /* Ignore singleton & */
	return 1;}
    if(!strcmp(argv[0], "pid")){ // if user enters pid then process id is printed
	printf("%d\n", getpid());
	return 1;
	}
    if(!strcmp(argv[0], "ppid")){ // if ppid is entered then parent process id is printed
	printf("%d\n", getppid());
	return 1;}
    if(!strcmp(argv[0], "help")){ // if help is entered then a list of built in commands are printed and their function is defined
	printf("Created by Jayanth Lankalapalli\n");
	printf("Built-in Commands:\n");
	printf("'exit' - to exit shell\n");
	printf("'pid' - to print the process id of the shell\n");
	printf("'ppid' - to print the parent process id of the shell\n");

	printf("'cd <path>' - to traverse to the repository\n");
	printf("'help' - to get list of built-in commands\n");
	printf("For non-built in commands enter 'man' for  assistance\n");
	return 1;
	}
     if(!strcmp(argv[0], "cd")){ // if cd is printed then that means to go to the directory stated  or show currnet directory location
	if(argv[1] == NULL){ // if it is null then that means only cd was entered and if that occurs then it prints current directory 
	getcwd(str, sizeof(str));
	printf("%s/",str);
	return 1;
	}

	a = chdir(argv[1]); // the a stores a numerical value of the chdir() function called and arg[1] is about the location entered by the user
	if(a != 0){ // if the int returned by the method call is not 0 then that means the directory could not be found to traverse so print error
	printf("Directory doesn't exist\n");
	}
	// if it did go to the direcotry print nothing the method call just moves it to that directory stated
	return 1;
      } 
   
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}
/* $end parseline */


