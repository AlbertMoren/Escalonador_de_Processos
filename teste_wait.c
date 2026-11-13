#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    printf("[PAI] Criando processo cliente...\n");

    int pid = fork();

    if (pid == 0) {
        // FILHO: vira o cliente
        execl("./cliente", "cliente", (char *)NULL);
        perror("Erro no execl");
        exit(1);
    }

    // PAI:
    int status;
    printf("[PAI] Esperando o cliente (PID %d) parar no SIGSTOP...\n", pid);

    // O waitpid bloqueia o pai até o filho parar ou morrer
    waitpid(pid, &status, WUNTRACED);

    if (WIFSTOPPED(status)) {
        printf("[PAI] Sucesso! O cliente parou (SIGSTOP).\n");

        // Agora que ele parou, o arquivo de demanda com certeza já foi escrito!
        FILE *f = fopen("demanda.txt", "r");
        int demanda = 0;
        if (f) {
            fscanf(f, "%d", &demanda);
            fclose(f);
            printf("[PAI] Demanda lida do arquivo: %d ms\n", demanda);
        }

        printf("[PAI] Agora vou descongelar o cliente com SIGCONT...\n");
        kill(pid, SIGCONT);

        printf("[PAI] Esperando o cliente terminar o atendimento...\n");
        waitpid(pid, &status, 0); // espera ele terminar de vez

        if (WIFEXITED(status)) {
            printf("[PAI] Cliente terminou o atendimento com sucesso!\n");
        }
    }

    return 0;
}
