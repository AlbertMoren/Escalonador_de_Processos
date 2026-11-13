#define N_THREADS 4
#include <stdio.h> 
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)

ucontext_t ctx_main;
ucontext_t ctx[N_THREADS];   // ctx[0] = A, ctx[1] = B, ctx[2] = C
int atual = 0;               // qual thread está rodando agora

void yield() {
    int anterior = atual;
    atual = (atual + 1)%N_THREADS;
    swapcontext(&ctx[anterior], &ctx[atual]);
}

void funcA() {
    for (int i = 1; i <= 3; i++) {
        printf("A%d\n", i);
        yield();      // ← não precisa mais saber quem é a próxima!
    }
}

void funcB(){
    for(int i = 1; i<= 3; i++){
    	printf("    B%d\n",i);
	yield();
    }
}

void funcC(){
    for(int i = 1; i <= 3; i++){
	printf("	C%d\n",i);
	yield();
    }
}

void funcD() {
    for (int i = 1; i <= 3; i++) {
        printf("	     D%d\n", i);
        yield();      // ← não precisa mais saber quem é a próxima!
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
    criar_thread(3, funcD);
    
    printf("main: vou começar\n");
    swapcontext(&ctx_main, &ctx[0]);
    printf("main: voltei!\n");
    return 0;
}
