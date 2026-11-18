#include <stdio.h>
#include <stdlib.h>
/* 
Algoritmo obligatorio: Bully, con la aptitud del Ejercicio 1 como prioridad.
a. (4 pts) Defina:
el enum de tipos de mensaje
el struct del dron, con los 4 criterios, la aptitud y el estado
el struct del mensaje, con tipo, origen y destino

b. (10 pts) Implemente el programa completo. Debe:
calcular F en C con la fórmula del Ejercicio 1
simular el intercambio de mensajes desde que K2 detecta la caída de K5 hasta que todos los drones operativos
reconocen al nuevo líder
imprimir cada mensaje con el formato emisor -> receptor : TIPO

c. (6 pts) Entregue:
la traza
el líder elegido
una explicación de por qué ganó, vinculada al algoritmo y a los valores de F
el número total de mensajes
*/

#define N 7

typedef enum { ELECTION, OK, COORDINATOR } TypeMsg;

typedef struct {
    int id;
    double bat, dist, vis, lat;
    double apt;
    int vivo;
    int en_eleccion;
} Dron;

typedef struct {
    TypeMsg tipo;
    int origen, destino;
} Mensaje;

Dron d[N] = {
{1, 55, 0.9, 80, 40, 0, 1, 0},
{2, 70, 1.8, 75, 25, 0, 1, 0},
{3, 88, 0.6, 62, 60, 0, 1, 0},
{4, 40, 0.4, 95, 30, 0, 1, 0},
{5, 0, 0.0, 0, 0, 0, 0, 0}, /* K5: lider caido */
{6, 76, 1.1, 90, 35, 0, 1, 0},
{7, 93, 2.4, 55, 20, 0, 1, 0},
};

const char *NOMBRE[] = {"ELECTION", "OK", "COORDINATOR"}; 
int lider = -1;
int cae = 0;

double aptitud(Dron x) {
    return 0.35 * (x.bat - 40) / 53 /* bateria: directa */
    + 0.20 * (2.4 - x.dist) / 2.0 /* distancia: inversa */
    + 0.30 * (x.vis - 55) / 40 /* visibilidad: directa */
    + 0.15 * (60 - x.lat) / 40; /* latencia: inversa */
}

void enviar(TypeMsg tipo, int o, int de) {
    Mensaje m = {tipo, o, de};
    printf("K%d -> K%d : %s\n", d[m.origen].id, d[m.destino].id, NOMBRE[m.tipo]);
    if (d[m.destino].vivo == 0) printf("sin respuesta\n");
    if (d[o].id == cae) {
    d[o].vivo = 0;
    printf("*** K%d se desconecta ***\n", cae);
    cae = 0;
    }
}

void eleccion(int i) {
    int hubo_ok = 0;
    d[i].en_eleccion = 1;
    for (int j = 0; j < N; j++) 
        if(d[i].vivo == 1 && d[j].apt > d[i].apt) {
            enviar(ELECTION, i, j);
            if(d[j].vivo == 1) {
                enviar(OK, j, i), hubo_ok = 1;
            }
        }
    if(d[i].vivo == 1 && hubo_ok == 0) {
        lider = i;
        for (int j = 0; j < N; j++)
            if (j != i && d[j].vivo == 1)
                enviar(COORDINATOR, i, j);
        return;
    }
    for (int j; j < N; j++)
        if(d[j].vivo == 1 && d[j].apt > d[i].apt && !d[j].en_eleccion)
            eleccion(j);
}

int main(int argc, char *argv[]) {
    if (argc > 1) cae = atoi(argv[1]);
    for (int i = 0; i < N; i++)
        d[i].apt = aptitud(d[i]);
    
    printf("K2 declara a K5 inoperativo");
    eleccion(1);
    printf("Nuevo lider: K%d (F = %.4f)\n", d[lider].id, d[lider].apt);
    return 0;
}

