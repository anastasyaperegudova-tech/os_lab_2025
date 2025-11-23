#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pids[10];
    int i;

    for (i = 9; i >= 0; --i) 
    {
        pids[i] = fork();
        if (pids[i] == 0) 
        {
            printf("child process №%d\n", i);
            sleep(i + 1);
            _exit(0);
        }
    }

    return 0;
}