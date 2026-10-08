#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* funcion stub */
char *get_estado(int sock, char *id_nave) {
    static char estado[64];
    char msg[128];
    int n;

    dprintf(sock, "CALL get_estado %s\n", id_nave);
    n = recv(sock, msg, sizeof msg - 1, 0);
    msg[n] = '\0';
    sscanf(msg, "RESULT %63s", estado);

    return estado;
}

int main(void) {
    char *naves[6] = {"Rocinante", "Canterbury", "Donnager", "Behemoth", "Nauvoo", "Tachi"};
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8001);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    for(int i = 0; i < 6; i++)
        printf("get_estado(%s) = %s\n", naves[i], get_estado(sock, naves[i]));

    close(sock);
    return 0;
}
