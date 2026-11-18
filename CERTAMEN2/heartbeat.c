#include <stdio.h>

#define PERIODO 5 /* s entre heartbeats */
#define TIMEOUT 2 /* s de espera por la respuesta */
#define N_FALLOS 3 /* ciclos consecutivos sin respuesta */

typedef struct {
    int objetivo;
    int fallos;
} Monitor;

int main(void) {
    int respuesta[] = {1, 0, 1, 1, 1, 1, 0, 0, 0};
    Monitor m = {3, 0};
    int t = 3 *3600 + 11 * 60 + 33; /* tiempo */

    for (int k = 0; k < 9; k++) {
        printf("[%02d:%02d:%02d] HB_REQ -> C%d ", t / 3600, t / 60 % 60, t % 60, m.objetivo);
        if (respuesta[k] == 1) {
            m.fallos = 0;
            printf("HB_ACK\n");
            t += PERIODO;
        } else {
            m.fallos++;
            printf("TIMEOUT (fallos = %d)\n", m.fallos);
            if (m.fallos == N_FALLOS) {
                printf("C%d INOPERATIVO -> iniciar eleccion\n", m.objetivo);
                break;
            }
            t += PERIODO + TIMEOUT;
        }
    }
    return 0;
}