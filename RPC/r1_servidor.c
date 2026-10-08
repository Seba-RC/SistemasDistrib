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
    char linea[128], id[32];

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8001);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(srv, (struct sockaddr *)&addr, sizeof(addr));
    listen(srv, 1);

    int cli = accept(srv, NULL, NULL);
    FILE *in = fdopen(cli, "r");

    while (fgets(linea, sizeof(linea), in)) {
        const char *estado = "DESCONOCIDA";
        id[0] = '\0';

        if (sscanf(linea, "CALL get_estado %31s", id) == 1) /* sscanf lee la linea, y si la linea coincide (si tiene el mismo formato), escribe a id*/
            for (int i = 0; i < 5; i++)
                if (strcmp(id, ids[i]) == 0) /* si la id coincide, entonces se entrega el estado */
                    estado = estados[i];
        printf("[SERVIDOR] get_estado(%s) -> %s\n", id, estado);
        dprintf(cli, "RESULT %s\n", estado);
    }

    fclose(in);
    close(srv);
    return 0;
}