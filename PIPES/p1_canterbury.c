#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main(void) {
    int fd[2], normal = 0, elevado = 0, critico = 0;
    char linea[64];
    const char *nivel;
    float v;

    setbuf(stdout, NULL);
    pipe(fd);

    if (fork() == 0) {
        close(fd[0]); /* No usamos el canal de lectura, solo escritura de hijo a padre */
        srand(time(NULL) ^ getpid());
        for(int i = 0; i < 10; i++) {
            dprintf(fd[1], "RAD %.2f\n", rand() / (double)RAND_MAX * 10); /* escribir mediante pipe de escritura (1) */
            sleep(1);
        }
        return 0;
    }

    close(fd[1]);
    FILE *in = fdopen(fd[0], "r"); /* leer archivo con RADs*/

    while(fgets(linea, sizeof linea, in)) {
        sscanf(linea, "RAD %f\n", &v); /* lee linea, con el formato indicado (RAD ...), y lo guarda en v */
        if (v < 3.0) {
            nivel = "NORMAL"; normal++;
        } else if (v < 6.0) {
            nivel = "ELEVADO"; elevado++;
        } else {
            nivel = "CRITICO"; critico++;
        }
    printf("[PUENTE] Lectura: %.2f mSv -> %s\n", v, nivel);
    }

    printf("RESUMEN CANTERBURY:\n");
    printf("Normal: %d\nElevado: %d\nCritico: %d", normal, elevado, critico);
}