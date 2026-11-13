#include <stdio.h>
#include <unistd.h>
int main() {
    int r = fork();
    if (r == 0) {
        printf("Sou o filho, meu PID = %d e meu pai = %d\n",getpid(),getppid());
    } else {
        printf("Sou o pai, meu PID = %d e criei o filho = %d\n",getpid(),r);
    }
    return 0;
}
