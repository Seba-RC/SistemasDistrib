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

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: ./sonda <ARES-N> <IP> <valor_inicial>\n");
        return 1;
    }

    const char *id = argv[1];
    const char *ip = argv[2];
    float valor = atof(argv[3]);
    char buf[BUF_SIZE], msg[BUF_SIZE], cmd[16];
    int n;

    /* TODO B.1  B.2 */
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int r = connect(sock, (struct sockaddr *)&addr, sizeof(addr));  
    if (r < 0) {                                                   
        perror("connect");
        return 1;
    }

    /* enviar CONNECT <id> y esperar HELLO o REJECT */
    snprintf(msg, sizeof msg, "CONNECT %s\n", id);
    send(sock, msg, strlen(msg), 0);

    n = recv(sock, buf, sizeof buf - 1, 0);
    if (n <= 0) {
        close(sock);
        return 1;
    }
    buf[n] = '\0';

    sscanf(buf, "%15s", cmd);
    if (strcmp(cmd, "HELLO") != 0) {
        fprintf(stderr, "Conexion rechazada: %s", buf);
        close(sock);
        return 1;
    }

    /* TODO B.3 */
    for(int i = 0; i < 8; i++) {
        char val[32], cs[3];
        int seq = i + 1;
        
        snprintf(val, sizeof(val), "%.1f", valor);
        checksum(val, cs);

        snprintf(msg, sizeof msg, "TELEM %s %d %s %s\n", id, seq, val, cs);
        send(sock, msg, sizeof buf - 1, 0);
        printf("Enviado: %s", msg);

        n = recv(sock, buf, sizeof buf - 1, 0);
        buf[n] = '\0';
        printf("Respuesta: %s", buf);

        valor += 1;
        sleep(1);
    }

    close(sock);
    return 0;
}