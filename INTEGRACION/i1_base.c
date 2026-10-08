#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int main(void) {
    struct sockaddr_in addr = {0};
    char linea[128], ts[16];

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(6001);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(srv, (struct sockaddr *)&addr, sizeof(addr));
    listen(srv, 1);

    int cli = accept(srv, NULL, NULL);
    FILE *in = fdopen(cli, "r");
    FILE *log = fopen("alertas.log", "a");
    while (fgets(linea, sizeof linea, in)) {
        linea[strcspn(linea, "\n")] = '\0';
 
        time_t t = time(NULL);
        strftime(ts, sizeof ts, "%H:%M:%S", localtime(&t));
 
        printf("[%s] [BASE] %s\n", ts, linea);
        fprintf(log, "[%s] %s\n", ts, linea);
        fflush(log);
 
        dprintf(cli, "ACK_ALERTA\n");
    }
 
    fclose(in);                                 /* cierra cli */
    fclose(log);
    close(srv);
    return 0;
}