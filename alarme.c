#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

int toques = 0;

void tratador(int sig) {
    toques++;
    printf("\n>>> DESPERTADOR! toque %d <<<\n", toques);
}

int main() {
    // 1. diz qual função roda quando chegar SIGALRM
    struct sigaction sa = {0};
    sa.sa_handler = tratador;
    sigaction(SIGALRM, &sa, NULL);

    // 2. programa o timer: primeiro toque em 500 ms, depois a cada 500 ms
    struct itimerval t;
    t.it_value.tv_sec     = 0;
    t.it_value.tv_usec    = 500000;   // 500.000 µs = 500 ms
    t.it_interval         = t.it_value;
    setitimer(ITIMER_REAL, &t, NULL);

    // 3. o main fica preso num loop infinito, e nunca chama tratador()
    long i = 0;
    while (toques < 5) {
        i++;
        if (i % 100000000 == 0) printf(".");
        fflush(stdout);
    }
    printf("\nmain: acabou depois de %d toques\n", toques);
    return 0;
}
