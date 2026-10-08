/* gcc -Wall -o r3_cliente r3_cliente.c
 * Uso: ./r3_cliente escritor | lector | fase */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* Stub: oculta la red. Envía "CALL <funcion> <args>" y retorna el valor de "RESULT <valor>". */
const char *rpc(int sock, const char *funcion, const char *args)
{
    static char res[128];
    char msg[256];
    int n;

    dprintf(sock, "CALL %s %s\n", funcion, args);

    n = recv(sock, msg, sizeof msg - 1, 0);
    msg[n] = '\0';

    if (sscanf(msg, "RESULT %127[^\n]", res) != 1)
        return "ERROR";
    return res;
}

int main(int argc, char **argv)
{
    const char *rol = argc > 1 ? argv[1] : "lector";
    struct sockaddr_in addr = {0};
    char args[64];
    int n = 0;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8003);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    srand(time(NULL) ^ getpid());

    if (strcmp(rol, "escritor") == 0) {             /* registra 12 muestras */
        for (int i = 1; i <= 12; i++) {
            snprintf(args, sizeof args, "%d %.2f", i, rand() / (double)RAND_MAX * 100);
            printf("[%s] registrar_muestra(%s) = %s\n", rol, args,
                   rpc(sock, "registrar_muestra", args));
            usleep(500000);
        }
    } else if (strcmp(rol, "lector") == 0) {        /* lee el estado cada segundo */
        for (int i = 0; i < 8; i++) {
            printf("[%s] leer_estado() = %s\n", rol, rpc(sock, "leer_estado", ""));
            sleep(1);
        }
    } else {                                        /* incrementa la fase al llegar a 10 muestras */
        while (1) {
            sscanf(rpc(sock, "leer_estado", ""), "fase=%*d muestras=%d", &n);
            if (n >= 10) {
                printf("[%s] incrementar_fase() = %s\n", rol, rpc(sock, "incrementar_fase", ""));
                break;
            }
            sleep(1);
        }
    }

    close(sock);
    return 0;
}