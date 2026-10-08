/* gcc -Wall -o s2_cliente s2_cliente.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void) {
    const char *msgs[5] = {"DOCK Rocinante AGUA\n", "DOCK Donnager COMBUSTIBLE\n", "DOCK Nauvoo OXIGENO\n", "DOCK Behemoth SILICATOS\n", "CAST_OFF\n"};
    struct sockaddr_in addr = {0};
    char buf[256];
    int n;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(7002);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    for (int i = 0; i < 5; i++) {
        printf("[CLIENTE] Envio: %s", msgs[i]);
        dprintf(sock, "%s", msgs[i]);

        n = recv(sock, buf, sizeof buf - 1, 0); /* espera respuesta */
        buf[n] = '\0';
        printf("[CLIENTE] Respuesta: %s", buf);
    }

    close(sock);
    return 0;
}