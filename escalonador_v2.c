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
int ativa[N_THREADS];
void (*funcoes[N_THREADS])();

void critica_fim() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);  // "destampa os ouvidos"
}

void critica_inicio() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);    // "tampa os ouvidos" para o alarme
}

void desligar_timer() {
    struct itimerval t = {0};
    setitimer(ITIMER_REAL, &t, NULL);
}

void yield() {
    critica_inicio();      

    int anterior = atual;
    int prox = -1;
    for (int i = 1; i <= N_THREADS; i++){
	int cand = (anterior + i) % N_THREADS;
	if (ativa[cand] == 1){
		prox = cand;
		break;
	}
    }


    if (prox == -1) {
	desligar_timer(); 
	critica_fim();
	setcontext(&ctx_main);
    }

    if (prox == anterior) {
	critica_fim();
	return;
    }
    
    atual = prox;
    swapcontext(&ctx[anterior], &ctx[prox]);
    critica_fim();  
}

void tratador(int sig) {     // ← agora o gcc já conhece o yield
    yield();
}

void funcA(){ 
	for (int k = 1; k <= 2; k++) {
        printf(" A%d ", k);
        fflush(stdout);
        for (volatile long j = 0; j < 150000000; j++);
    }
    printf(" [A fim] ");
    fflush(stdout);
}

void funcB(){
	for (int k = 1; k <= 5; k++) {
        printf(" B%d ", k);
        fflush(stdout);
        for (volatile long j = 0; j < 150000000; j++);
    }
    printf(" [A fim] ");
    fflush(stdout);
}

void funcC(){
	for (int k = 1; k <= 3; k++) {
        printf(" C%d ", k);
        fflush(stdout);
        for (volatile long j = 0; j < 150000000; j++);
    }
    printf(" [A fim] ");
    fflush(stdout);
}


void rodar_thread() {
    funcoes[atual]();
    ativa[atual] = 0;
    yield();
}

void criar_thread(int id, void (*func)()) {
    getcontext(&ctx[id]);
    ctx[id].uc_stack.ss_sp   = malloc(STACK_SIZE);
    ctx[id].uc_stack.ss_size = STACK_SIZE;
    ctx[id].uc_link          = NULL;
    funcoes[id]               = func;
    ativa[id]                 = 1;
    makecontext(&ctx[id], rodar_thread, 0);
}

int main() {
    criar_thread(0, funcA);
    criar_thread(1, funcB);
    criar_thread(2, funcC);
    
    struct sigaction sa = {0};
    sa.sa_handler = tratador;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);
    
     struct itimerval t;
     t.it_value.tv_sec     = 0;
     t.it_value.tv_usec    = 100000;
     t.it_interval         = t.it_value;
     setitimer(ITIMER_REAL, &t, NULL);

    printf("main: vou começar\n");
    swapcontext(&ctx_main, &ctx[0]);
    printf("main: voltei!\n");
    return 0;
}
