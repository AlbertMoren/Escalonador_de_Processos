#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Sou o PAI (PID %d), vou criar um filho cliente...\n", getpid());

    int pid_filho = fork();

    if (pid_filho == 0) {
        // Código do FILHO:
        printf("Filho (PID %d): Vou me transformar no ./cliente agora!\n", getpid());
        execl("./cliente", "cliente", (char *)NULL);

        // Se chegar aqui, deu erro (ex: arquivo ./cliente não existe)
        perror("Erro no execl");
        exit(1);
    } else {
        // Código do PAI:
        printf("Pai: Criei o filho com PID = %d. Vou esperar 2 segundos e ver o que aconteceu.\n", pid_filho);
        sleep(2);
        printf("Pai: Fim do teste.\n");
    }

    return 0;
}
