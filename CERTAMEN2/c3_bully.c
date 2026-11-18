/* funcionamiento del algoritmo:
- Cada dron envia ELECTION a drones con una F mayor a la suya
- Quien recibe el ELECTION responde OK e inicia su eleccion
- Quien no tiene a nadie con mayor F, se proclama lider y envia COORDINATOR a todos
- Si hay empate de F gana el ID mayor
*/

/* c03_bully.c - Respuesta modelo Control 03: Bully con aptitud F (Boveda Sigma-7) */
/* c03_bully.c - Respuesta modelo Control 03: Bully con aptitud F (Boveda Sigma-7) */
#include <stdio.h>
#include <stdlib.h>
#define N 6

typedef enum { ELECTION, OK, COORDINATOR } TipoMsg;

typedef struct {
    int id;
    double bat, dist, vis; /* criterios */
    double apt; /* aptitud F */
    int vivo; /* 1 operativo, 0 caido */
    int en_eleccion; /* 1 si ya inicio su eleccion */
    } Dron;

typedef struct {
    TipoMsg tipo;
    int origen, destino; /* indices en el arreglo d[] */
    } Mensaje;

Dron d[N] = {
    {1, 78, 1.2, 85, 0, 1, 0},
    {2, 45, 0.8, 92, 0, 1, 0},
    {3, 0, 0.0, 0, 0, 0, 0}, /* C3: lider caido */
    {4, 91, 2.1, 70, 0, 1, 0},
    {5, 62, 0.5, 88, 0, 1, 0},
    {6, 83, 1.5, 45, 0, 1, 0},
};
const char *NOMBRE[] = {"ELECTION", "OK", "COORDINATOR"};

int lider = -1;
int cae = 0; /* solo Ej. 6: id del dron que cae tras enviar su primer mensaje */

double aptitud(Dron x) { /* F de la Pregunta 1, min y max de la tabla */
    return 0.25 * (x.bat - 45) / 46 /* bateria: directa */
    + 0.25 * (2.1 - x.dist) / 1.6 /* distancia: inversa */
    + 0.50 * (x.vis - 45) / 47; /* visibilidad: directa */
    }
void enviar(TipoMsg tipo, int o, int de) {
    Mensaje m = {tipo, o, de};

    printf("C%d -> C%d : %s", d[m.origen].id, d[m.destino].id, NOMBRE[m.tipo]);
    if (!d[m.destino].vivo) printf(" (sin respuesta)");
        printf("\n");
    if (d[o].id == cae) {
        d[o].vivo = 0;
        printf("*** C%d se desconecta ***\n", cae);
        cae = 0;
    }
}

void eleccion(int i) {
    int hubo_ok = 0;
    d[i].en_eleccion = 1;
    for (int j = 0; j < N; j++) 
        if(d[i].vivo == 1 && d[j].apt > d[i].apt) {
            enviar(ELECTION, i, j);
            if (d[j].vivo == 1) {
                enviar(OK, j, i); hubo_ok = 1;
            }
        }
    if(d[i].vivo && !hubo_ok) {
            lider = i;
            for (int j = 0; j < N; j++)
                if(j != i && d[j].vivo == 1)
                    enviar(COORDINATOR, i, j);
            return; 
        }
    for(int j = 0; j < N; j++)
        if(d[j].vivo == 1 && d[j].apt > d[i].apt && !d[j].en_eleccion)
            eleccion(j);
}

int main(int argc, char *argv[]) {
    if (argc > 1) cae = atoi(argv[1]);
    for (int i = 0; i < N; i++)
        d[i].apt = aptitud(d[i]);

    printf("C1 declara a C3 inoperativo (3 heartbeats sin respuesta)\n");
    eleccion(0);
    printf("Nuevo lider: C%d (F = %.4f)\n", d[lider].id, d[lider].apt);
    return 0;
}