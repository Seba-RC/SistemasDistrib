/* gcc -Wall -o s2_ceres s2_ceres_corregido.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void) {
    const char *tipos[4] = {"AGUA", "SILICATOS", "OXIGENO", "COMBUSTIBLE"};
    const char *bahias[4] = {"Bahia-A", "Bahia-B", "Bahia-C", "Bahia-D"};
    struct sockaddr_in addr = {0};
    char linea[128], nave[32], carga[32];

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(7002);

    signal(SIGCHLD, SIG_IGN); /* evita zombis */

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(srv, (struct sockaddr *)&addr, sizeof(addr));
    listen(srv, 5);

    while (1) {
        int cli = accept(srv, NULL, NULL);

        if (fork() == 0) {
            close(srv);
            FILE *in = fdopen(cli, "r");

            while (fgets(linea, sizeof linea, in)) {
                if (strncmp(linea, "CAST_OFF", 8) == 0) { /* "si la linea recibida es igual a CAST_OFF (8 caracteres), entonces .. "*/
                    dprintf(cli, "BYE\n");
                    break;
                }

                const char *bahia = NULL;
                if (sscanf(linea, "DOCK %s %s", nave, carga) == 2) { /* sscanf retorna un numero que dice cuantos valores se leyeron de una linea; si son 2, entonces pasa a..  */
                    for (int i = 0; i < 4; i++) {
                        if (strcmp(carga, tipos[i]) == 0) {
                            bahia = bahias[i];
                        }
                    }
                }
                if (bahia) {
                    printf("[CERES] %s -> %s\n", nave, bahia);
                    dprintf(cli, "ASIGNADO %s\n", bahia);
                } else {
                    dprintf(cli, "ERROR\n");
                }
            }

            fclose(in); /* cierra cli (cliente) */
            return 0;
        }

        close(cli); /* padre ya no usa socket cli, se cierra */
    }
}