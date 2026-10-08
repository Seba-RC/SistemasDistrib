#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main(void) {
    int p1[2], p2[2];
    char linea[64];
    float v;

    pipe(p1);
    pipe(p2);

    if (fork() == 0) { /* escribir valor en p1 */
        close(p1[0]); close(p2[0]); close(p2[1]);
        srand(time(NULL) ^ getpid());
        for(int i = 0; i < 15; i++) {
            dprintf(p1[1], "TEMP %.2f\n", -50 + rand()/(double)RAND_MAX*350); /* escribir a p1 */
        }
        close(p1[1]);
        return 0;
    }

    if (fork() == 0) { /* lee desde p1, escribe en p2; filtro */
        close(p1[1]); close(p2[0]);
        FILE *in = fdopen(p1[0], "r");
        while (fgets(linea, sizeof linea, in)) {
            sscanf(linea, "TEMP %f\n", &v);
            if (v >= 20 && v <= 200) {
                dprintf(p2[1], "TEMP_OK %.2f\n", v);
            } else {
                printf("[FILTRO] Descartado: %.2f\n", v);
            }
        }
        close(p1[0]); close(p2[1]);
        return 0;
    }

    if (fork() == 0) { /* lee desde p2; imprime valores validos */
        close(p1[0]); close(p1[1]); close(p2[1]);
        FILE *in = fdopen(p2[0], "r");
        while (fgets(linea, sizeof linea, in)) {
            sscanf(linea, "TEMP_OK %f\n", &v);
            printf("[PUENTE] Temperatura operacional:%.2f\n", v);
        }
        close(p2[0]);
        return 0;
    }
    close(p1[0]); close(p1[1]); close(p2[0]); close(p2[1]);
    return 0;
}