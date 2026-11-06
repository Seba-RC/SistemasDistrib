#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 7070
#define BUF_SIZE 256
char *checksum(const char *valor, char out[3]) {
    unsigned int s = 0;
    for (int i = 0; valor[i]; i++) s += (unsigned char)valor[i];
    snprintf(out, 3, "%02X", s % 256);
    return out;
}
typedef struct {
    char id[8];
    float alerta;
    float critico;
    int mayor; /* 1: alerta si valor > umbral | 0: si valor < umbral */
} Umbral;


Umbral umbrales[] = {
    {"ARES-1", -40, -30, 1},
    {"ARES-2", 3.5, 6.0, 1},
    {"ARES-3", 750, 900, 1},
    {"ARES-4", 30, 15, 0},
};


void handle_sonda(int fd) {
    char buf[BUF_SIZE], msg[BUF_SIZE];
    char cmd[16], id[8] = "", tid[8], val[32], cs[8], calc[3];
    int n, seq;
    Umbral *u = NULL;

    /* TODO A.1 */
    n = recv(fd, buf, sizeof buf - 1, 0);
    buf[n] = '\0';

    sscanf(buf, "%15s %7s", cmd, id); /* scan buf, guardar en cmd e id */

    if (strcmp(cmd, "CONNECT") == 0) {
        if (strcmp(id, "ARES-1") == 0) u = &umbrales[0];
        else if (strcmp(id, "ARES-2") == 0) u = &umbrales[1];
        else if (strcmp(id, "ARES-3") == 0) u = &umbrales[2];
        else if (strcmp(id, "ARES-4") == 0) u = &umbrales[3];
    }

    if (u == NULL) {
        snprintf(msg, sizeof(msg), "REJECT %s\n", id);
        send(fd, msg, strlen(msg), 0);
        close(fd);
        return;
    }

    snprintf(msg, sizeof(msg), "HELLO %s\n", id); /* escribir a msg */
    send(fd, msg, strlen(msg), 0); /* mandar msg, mediante fd */

    /* TODO A.2 */
    while ((n = recv(fd, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[n] = '\0';
        seq = 0;

        /* rechazo */
        if (sscanf(buf, "%s %s %d %s %s", cmd, tid, &seq, val, cs) != 5
        || strcmp(cmd, "TELEM") != 0 || strcmp(tid, id) != 0) {
            snprintf(msg, sizeof msg, "NACK %s %d\n", id, seq);
            send(fd, msg, strlen(msg), 0);
            continue;
        }

        checksum(val, calc);
        if (strcmp(calc, cs) != 0) {
            snprintf(msg, sizeof msg, "NACK %s %d\n", id, seq);
            send(fd, msg, strlen(msg), 0);
            continue;
        }
        /* aceptacion */
        snprintf(msg, sizeof msg, "ACK %s %d\n", id, seq);
        send(fd, msg, strlen(msg), 0);

        /* evaluacion umbrales */
        float v = atof(val);
        char *nivel = NULL;

        if (u->mayor == 1) {
            if (v > u->critico) nivel = "CRITICO";
            else if (v > u->alerta) nivel = "ALERTA";
        } else {
            if (v < u->critico) nivel = "CRITICO";
            if (v < u->alerta) nivel = "ALERTA";
        }
        printf(nivel);
    }
    close(fd);
}

int main(void) {
    /* TODO A.3 */
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(srv, (struct sockaddr *)&addr, sizeof(addr));
    listen(srv, 5);

    while(1) {
        int cli = accept(srv, NULL, NULL);

        if (fork() == 0) {
            close(srv);
            handle_sonda(cli);
            exit(0);
        }

        close(cli);
        waitpid(-1, NULL, WNOHANG);
    }
    return 0;
}