/* gcc -Wall -o r1_servidor r1_servidor.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void) {
    const char *estados[3] = {"OPTIMO", "DEGRADADO", "CRITICO"};
    struct sockaddr_in addr = {0}
    char linea[128];
    int n;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8002);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(srv, (struct sockaddr *)&addr, sizeof(addr));
    listen(srv, 1);

    int cli = accept(srv, NULL, NULL);
    FILE *in = fdopen(cli, "r");

    while fgets(linea, sizeof(linea), in) {
        /* ... */
    }
}