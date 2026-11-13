#include <stdio.h>
#include <unistd.h>

int main(){
	printf("Ants do fork. Meu PID = %d\n", getpid());
	
	int r = fork();

	printf("Depois do fork, Meu PID = %d, r = %d\n",getpid(), r);
	
	return 0;
}
