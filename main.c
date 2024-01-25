#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define N_FIGLI 8

int tabpid[N_FIGLI];

/*
 * 2. selezione della politica della gestione dei segnali
 *    gestione inaffidabile del segnale
 */

void handler (int signo) {
    printf("Processo %d e ho ricevuto il segnale %d\n", getpid(), signo);
}

int main (int argc, char *argv[]) {
    int piped[2], pid, ppid, status;

    if (argc != 2) {
        printf("Numero di argomenti incorretto");
        exit(1);
    }

    // punto 2
    signal(SIGUSR1, handler);

    // 3. creazione della pipe
    if (pipe(piped) < 0) {
        perror("Errore nella creazione della pipe");
        exit(2);
    }

    // 4. creazione del processo figlio
    for (int i = 0; i < N_FIGLI; i++) {
        if ((pid = fork()) < 0) {
            perror("Errore nella fork");
            exit(3);
        }
        else if (tabpid[i] == 0) {
            // Codice del figlio

            if (argv[1] == "a") {
                if (i % 2) { pause(); } // caso di ID dispari
                else {
                    // id pari

                    /*
                     * dato che il messaggio una volta letto viene "consumato"
                     * il messaggio del tabpid va reinserito all'interno della pipe
                     */

                    read(piped[0], tabpid, sizeof tabpid);
                    write(piped[1], tabpid, sizeof tabpid);

                    kill(tabpid[i + 1], SIGUSR1);
                }
            }
            else if (argv[1] == "b") {
                if (i >= N_FIGLI/2) { pause(); }
                else {
                    read(piped[0], tabpid, sizeof tabpid);
                    write(piped[1], tabpid, sizeof tabpid);

                    kill(tabpid[i + N_FIGLI/2], SIGUSR1);
                }
            }
            printf("Completato");
            exit(0);
        }
        else {
            // Codice del padre

            /*
             * il padre deve mettere la tabpid nella pipe per permettere ai processi figli di comunicare
             */

            printf("PADRE: scrivo sulla pipe la tabella dei pid\n");
            write(piped[1], tabpid, sizeof tabpid);
        }
    }
}