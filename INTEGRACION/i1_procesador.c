#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
 
/* envía una alerta a la base y espera el ACK */
void alerta(int sock, const char *tipo, const char *campo, float valor)
{
    char buf[64];
    int n;
 
    dprintf(sock, "%s %s=%.1f\n", tipo, campo, valor);
 
    n = recv(sock, buf, sizeof buf - 1, 0);
    buf[n] = '\0';
    printf("[PROCESADOR] %s %s=%.1f -> %s", tipo, campo, valor, buf);
}
 
int main(void)
{
    struct sockaddr_in addr = {0};
    char linea[128];
    float o2, co2, temp;
    int fd[2], alertas;
 
    pipe(fd);
 
    if (fork() == 0) {                          /* A: sensor */
        close(fd[0]);
        srand(time(NULL) ^ getpid());
        for (int i = 0; i < 10; i++) {
            dprintf(fd[1], "VITAL O2=%.1f CO2=%.1f TEMP=%.1f\n",
                    80 + rand() / (double)RAND_MAX * 20,
                    rand() / (double)RAND_MAX * 5,
                    15 + rand() / (double)RAND_MAX * 10);
            sleep(2);
        }
        return 0;
    }
 
    close(fd[1]);                               /* B: procesador, solo lee el pipe */
 
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(6001);
 
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
 
    FILE *in = fdopen(fd[0], "r");
 
    while (fgets(linea, sizeof linea, in)) {
        sscanf(linea, "VITAL O2=%f CO2=%f TEMP=%f", &o2, &co2, &temp);
        alertas = 0;
 
        if (o2 < 85)   { alerta(sock, "ALERTA_O2",   "O2",   o2);   alertas++; }
        if (co2 > 3)   { alerta(sock, "ALERTA_CO2",  "CO2",  co2);  alertas++; }
        if (temp > 22) { alerta(sock, "ALERTA_TEMP", "TEMP", temp); alertas++; }
 
        if (!alertas)
            printf("[PROCESADOR] O2=%.1f CO2=%.1f TEMP=%.1f -> normal\n", o2, co2, temp);
    }
 
    close(sock);                                /* cierra la conexión con la base */
    fclose(in);                                 /* y luego pipe[0] del sensor */
    wait(NULL);
    return 0;
}