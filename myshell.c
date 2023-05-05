#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

// Used for pipeing
static int grandson_pid = 0;

// This is the main signal handler of the parent, we ignore SIGINT and SIGCHLD
int prepare(void) {
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {perror("Error - SIGINT handling"); exit(1);}
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {perror("Error - SIGCHLD handling"); exit(1);}
    return 0;
}
int finalize(void) {return 0;}

// Handels the case that there is piping operator
void pipe_func(char** arglist, int pipe_idx, int* pfds) {
    int g = fork();
            
    // son case (he is the grandon of the main proccess)
    if (g == 0) {
    close(pfds[1]);
    dup2(pfds[0], STDIN_FILENO);
    execvp(arglist[pipe_idx], arglist + pipe_idx);
    perror("Error in Piping, try again master"); exit(EXIT_FAILURE);
    }

    // parent case (he is the son of the main proccess)
    if (g > 0) {
    grandson_pid = g;
    close(pfds[0]);
    dup2(pfds[1], STDOUT_FILENO);
    execvp(arglist[0], arglist);
    perror("Error in Piping, try again master"); exit(EXIT_FAILURE);
    }
}

int process_arglist(int count, char** arglist) {
    int concurrent = 0;
    int pipe_idx = 0;
    int pfds[2];

    // concurrent case
    if (arglist[count-1][0] == '&') {  concurrent = 1; arglist[--count] = NULL;  }

    // check pipe operator   
    else {
        for (int i = 0; i < count; i++) {
            if (arglist[i][0] == '|') {pipe_idx = i + 1;  arglist[i] = NULL; break;}
        }
        // we found the piping operator so we create the pipe for the sons of this proccess
        if (pipe_idx > 0 && pipe(pfds) == -1) {
            perror("Error in Piping, please try again master\n");
        }
    }

    int f = fork();
    

    //son code
    if (f == 0) {
        // signal handler for children, treminate proccess with SIGINT if proccess is foregroung proccess
        if (concurrent == 0 && signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("Error - failed to change signal SIGINT handling"); exit(1);
    }

        // pipe case
        if (pipe_idx > 0) {pipe_func(arglist, pipe_idx, pfds);}

        // redirection case
        if (count > 2 && arglist[count - 2][0] == '<') {
            char* file = arglist[--count]; arglist[--count] = NULL;

            int fd = open(file, O_RDONLY);
            if (fd < 0) {perror("Error in file Adress"); exit(1);}

            dup2(fd, STDIN_FILENO); 
        }    

        execvp(arglist[0], arglist);
        perror("Execvp error"); exit(1); 
    }

    //parent code
    if (f > 0) {
        // if pipe was created
        if (pipe_idx){
            close(pfds[0]); close(pfds[1]);
            waitpid(grandson_pid, NULL, 0); waitpid(f, NULL, 0);
        }
        // non concurrent case
        if (!concurrent) {waitpid(f, NULL, 0);}
    }

    return 1;
}