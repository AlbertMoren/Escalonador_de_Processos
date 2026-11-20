#define N_THREADS 3
#include <stdio.h> 
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>

#define STACK_SIZE (64 * 1024)

ucontext_t ctx_main;
ucontext_t ctx[N_THREADS];   // ctx[0] = A, ctx[1] = B, ctx[2] = C
int atual = 0;               // qual thread está rodando agora

void yield() {
    int anterior = atual;
    atual = (atual + 1)%N_THREADS;
    swapcontext(&ctx[anterior], &ctx[atual]);
}

void tratador(int sig) {     // ← agora o gcc já conhece o yield
    yield();
}

void funcA() {
    long i = 0;
    while (1) {
        i++;
        if (i % 100000000 == 0) { printf(" A "); fflush(stdout); }
    }
}

void funcB(){
    long i = 0;
    while (1) {
        i++;
        if (i % 100000000 == 0) { printf(" B "); fflush(stdout); }
    }
}

void funcC(){
    long i = 0;
    while (1) {
        i++;
        if (i % 100000000 == 0) { printf(" C "); fflush(stdout); }
    }
}


void criar_thread(int id, void (*func)()) {
    getcontext(&ctx[id]);
    ctx[id].uc_stack.ss_sp   = malloc(STACK_SIZE);
    ctx[id].uc_stack.ss_size = STACK_SIZE;
    ctx[id].uc_link          = &ctx_main;
    makecontext(&ctx[id], func, 0);
}


int main() {

    criar_thread(0, funcA);
    criar_thread(1, funcB);
    criar_thread(2, funcC);
    
    struct sigaction sa = {0};
    sa.sa_handler = tratador;
    sigaction(SIGALRM, &sa, NULL);
    
     struct itimerval t;
     t.it_value.tv_sec     = 1;
     t.it_value.tv_usec    = 100000;
     t.it_interval         = t.it_value;
     setitimer(ITIMER_REAL, &t, NULL);

    printf("main: vou começar\n");
    swapcontext(&ctx_main, &ctx[0]);
    printf("main: voltei!\n");
    return 0;
}
