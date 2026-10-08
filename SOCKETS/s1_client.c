#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void) {
    struct sockaddr_in addr = {0};
    char buf[256];
    int n;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(7001);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    for (int i = 1; i <= 5; i++) {
        dprintf(sock, "MSG nave_OPA_1 dato_%d\n", i); /* escribe el msg en el socket */

        n = recv(sock, buf, sizeof buf - 1, 0); /* espera ACK */
        buf[n] = '\0';
        printf("[CLIENTE] Respuesta: %s", buf);
    }
    close(sock);
    return 0;
}