/* gcc -Wall -o r1_servidor r1_servidor.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void) {
    const char *ids[5] = {"Rocinante", "Cantersbury", "Donnager", "Behemoth", "Nauvoo"};
    const char *estados[5] = {"EN_VUELO", "DESTRUIDA", "EN_PUERTO", "EN_VUELO", "CONSTRUCCION"};
    struct sockaddr_in addr = {0};
    char buf[128], msg[128], id[32];
    int n;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8001);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(srv, (struct sockaddr *)&addr, sizeof(addr));
    listen(srv, 1);

    int cli = accept(srv, NULL, NULL);

    while ((n = recv(cli, buf, sizeof buf - 1, 0)) > 0) {
        const char *estado = "DESCONOCIDA";
        buf[n] = '\0';
        id[0] = '\0';

        if (sscanf(buf, "CALL get_estado %31s", id) == 1)
            for (int i = 0; i < 5; i++)
                if (strcmp(id, ids[i]) == 0)
                    estado = estados[i];

        printf("[SERVIDOR] get_estado(%s) -> %s\n", id, estado);

        snprintf(msg, sizeof msg, "RESULT %s\n", estado);
        send(cli, msg, strlen(msg), 0);
    }

    close(cli);
    close(srv);
    return 0;
}