#include <stdio.h>
#include <stdlib.h>

#define N 7

typedef enum { ELECTION, COORDINATOR } TipoMsg;

typedef struct {
    int id;
    double apt; /* F calculada en la Pregunta 1 */
    int vivo;
    } Dron;

typedef struct {
    TipoMsg tipo;
    int origen, destino;
    int mejor; /* indice del dron con mayor F visto hasta ahora */
    } Mensaje;

Dron d[N] = { /* orden del anillo = orden por id */
{1, 0.5116, 1},
{2, 0.5394, 1},
{3, 0.5495, 1},
{4, 0.6125, 1},
{5, 0.0, 0}, /* K5: lider caido */
{6, 0.7240, 1},
{7, 0.5000, 1}
};

int sucesor(int i) {
    int s = (i + 1) % N;
    while (!d[s].vivo) {
        printf( "K%d no responde (timeout): se salta\n", d[s].id);
        s = (s + 1) % N;
    }
    return s;
}

void enviar(Mensaje m) {
    printf("K%d -> K%d : %s (mejor = K%d)\n", d[m.origen].id, d[m.destino].id,
    m.tipo == ELECTION ? "ELECTION" : "COORDINATOR", d[m.mejor].id);
}

int main(int argc, char *argv[]) {
    int cae = (argc > 1) ? atoi(argv[1]) : 0; 
    int ini = 1, i = ini;
    Mensaje m = {ELECTION, ini, ini, ini};

    printf("K2 declara a K5 inoperativo e inicia la eleccion\n");
    do {
        m.origen = i;
        m.destino = sucesor(i);
        enviar(m);
        i = m.destino;
        if (d[i].apt > d[m.mejor].apt) m.mejor = i;
    } while (i != ini);

    printf("K2 recibe su ELECCION de vuelta: el lider es K%d\n", d[m.mejor].id);
    if (cae) { d[cae - 1].vivo = 0; printf("***K%d se desconecta\n***", cae); }

    m.tipo = COORDINATOR;
    do {
        m.origen = i;
        m.destino = sucesor(i);
        enviar(m);
        i = m.destino;
    } while (i != ini);

    printf("Nuevo lider: K%d (F = %.4f)\n", d[m.mejor].id, d[m.mejor].apt);
    return 0;
}
