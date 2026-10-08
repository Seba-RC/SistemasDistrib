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
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(7001);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(srv, (struct sockaddr*)&addr, sizeof(addr));
    listen(srv, 1);

    int cli = accept(srv, NULL, NULL); /* aceptar cliente*/

    while((n = recv(cli, buf, sizeof buf - 1, 0)) > 0) { /* recibe desde cliente, escribe en buf, size maximo buf - 1; se ejecuta mientras reciba mensajes ( > 0)*/
        buf[n] = '\0'; /* limpieza mensaje*/
        buf[strcspn(buf, "\n")] = '\0';

        printf("[RELAY] Recibido: %s\n", buf);
        dprintf(cli, "ACK %s\n", buf);
    }

    close(cli);
    close(srv);
    return 0;
}