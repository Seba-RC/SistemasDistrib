#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main(void) {
    int fd[2], n, normal = 0, elevado = 0, critico = 0;
    char buf[64], msg[32];
    const char *nivel;
    float v;

    setbuf(stdout, NULL);
    pipe(fd);

    if (fork() == 0) {
        close(fd[0]);                 /* el hijo solo escribe */
        srand(time(NULL) ^ getpid());
        for (int i = 0; i < 10; i++) {
            snprintf(msg, sizeof msg, "RAD %.2f\n", rand() / (double)RAND_MAX * 10);
            write(fd[1], msg, strlen(msg));
            sleep(1);
        }
        close(fd[1]);                 /* cierra el extremo: el padre recibe fin de archivo */
        return 0;
    }

    close(fd[1]);                     /* el padre solo lee */

    while ((n = read(fd[0], buf, sizeof buf - 1)) > 0) {
        buf[n] = '\0';
        sscanf(buf, "RAD %f", &v);

        if (v < 3.0) {
            nivel = "NORMAL"; normal++;
        } else if (v < 6.0) {
            nivel = "ELEVADO"; elevado++;
        } else {
            nivel = "CRITICO"; critico++;
        }
        printf("[PUENTE] Lectura: %.2f mSv -> %s\n", v, nivel);
    }

    close(fd[0]);

    printf("RESUMEN CANTERBURY:\n");
    printf("Normal: %d\nElevado: %d\nCritico: %d\n", normal, elevado, critico);
    return 0;
}