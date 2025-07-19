// Inclusao da bibliotecas
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    // Declaracao dos identificadores dos processos filhos
    pid_t f1,f2,n1,n2,n3,n4;

    // Processo pai (P1) inicia

    // Criacao do primeiro filho F1
    f1 = fork();

    // Se falhar exibe a mensagem e encerra o programa
    if (f1 < 0) { 
        perror("Erro ao criar processo.");
        exit(1);
    } else if(f1 == 0) { 
        // Processo F1 em execucao

        // Cria N1 filho de F1
        n1 = fork(); 

        // Se falhar exibe a mensagem e encerra o programa
        if (n1 < 0) {
            perror("Erro ao criar processo.");
            exit(1);
        } else if(n1 == 0) {
            // Processo N1
            printf("N1 Exibe uma mensagem: ");
            // Exibe uma mensagem
            execl("/bin/echo", "echo", "Atividade de SO", NULL);
            printf("\n");

            // Caso o execl falhar
            perror("execl N1.");
            exit(1);
        }
        
        // Cria N2 filho de F1
        n2 = fork();

        // Se falhar exibe a mensagem e encerra o programa
        if (n2 < 0) {
            perror("Erro ao criar processo.");
            exit(1);
        } else if(n2 == 0) {
            // Processo N2
            printf("N2 Imprime o nome do host da maquina: ");
            // Imprime o nome do host da maquina
            execl("/bin/hostname", "hostname", NULL);
            printf("\n");

            // Caso execl falhar
            perror("execl N2.");
            exit(1);
        }
        
        // F1 espera N1 e N2 terminarem
        waitpid(n1, NULL, 0);
        waitpid(n2,NULL, 0);

        // Apos os dois terminarem exibe a mensagem
        printf("Processo F1 (PID: %d, PPID: %d) finalizou apos N1 e N2.\n", getpid(), getppid());
        exit(0);
    }

    // Processo P1 continua

    // Criacao do segundo filho F2
    f2 = fork();

    // Se falhar exibe a mensagem e encerra o programa
    if (f2 < 0) {
        perror("Erro ao criar processo.");
        exit(1);
    } else if (f2 == 0) {
        // Processo F2 executando

        // Cria N3 filho de F2
        n3 = fork();

        // Se falhar exibe a mensagem e encerra o programa
        if (n3 < 0) {
            perror("Erro ao criar processo.");
            exit(1);
        } else if(n3 == 0) {
            // Processo N3
            printf("N3 Exibe data e hora atual: ");
            // Exibe data e hora atual
            execl("/bin/date", "date", NULL);
            printf("\n");
            
            // Caso o execl falhar
            perror("execl N3.");
            exit(1);
        }
        
        // Cria N4 filho de F2
        n4 = fork();

        // Se falhar exibe a mensagem e encerra o programa
        if (n4 < 0) {
            perror("Erro ao criar processo.");
            exit(1);
        } else if(n4 == 0) {
            // Processo N4
            printf("N4 Imprime o nome do usuario atual: " );
            // Imprime o nome do usuario atual
            execl("/usr/bin/whoami", "whoami", NULL);
            printf("\n");

            // Caso execl falhar
            perror("execl N4.");
            exit(1);
        }
        
        // F1 espera N1 e N2 terminarem
        waitpid(n3, NULL, 0);
        waitpid(n4,NULL, 0);

        // Apos os dois terminarem exibe a mensagem
        printf("Processo F2 (PID: %d, PPID: %d) finalizou apos N1 e N2.\n", getpid(), getppid());
        exit(0);
    }

    // Processo P1 espera F1 e F2 
    waitpid(f1, NULL, 0);
    waitpid(f2, NULL, 0);

    printf("Processo P1 (PID: %d) finalizou apos F1 e F2.\n", getpid());

    return 0;
}