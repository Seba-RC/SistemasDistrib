/* gcc -Wall -o r3_ilus r3_ilus.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ARCHIVO "experimento.dat"

typedef const char *(*handler_fn)(char *args);

/* contenido del archivo: fase, cantidad de muestras y hasta 50 muestras */
struct Exp {
    int fase;
    int num;
    struct { int id; double valor; } m[50];
};

/* cada llamada abre su propio descriptor: así cada proceso tiene su propio lock */
int abrir(int op)               { int fd = open(ARCHIVO, O_RDWR); flock(fd, op); return fd; }
void cargar(int fd, struct Exp *e)  { lseek(fd, 0, SEEK_SET); read(fd, e, sizeof *e); }
void guardar(int fd, struct Exp *e) { lseek(fd, 0, SEEK_SET); write(fd, e, sizeof *e); }
void cerrar(int fd)             { flock(fd, LOCK_UN); close(fd); }

/* ---------- handlers ---------- */

const char *registrar_muestra(char *args)               /* escritura: LOCK_EX */
{
    static char res[64];
    struct Exp e;
    int id;
    double valor;

    if (sscanf(args, "%d %lf", &id, &valor) != 2)
        return NULL;

    int fd = abrir(LOCK_EX);
    cargar(fd, &e);
    if (e.num >= 50) {
        cerrar(fd);
        return "LLENO";
    }
    e.m[e.num].id = id;
    e.m[e.num].valor = valor;
    e.num++;
    guardar(fd, &e);
    cerrar(fd);

    snprintf(res, sizeof res, "OK %d", e.num);
    return res;
}

const char *leer_estado(char *args)                     /* lectura: LOCK_SH */
{
    static char res[128];
    struct Exp e;
    double suma = 0;

    (void)args;
    int fd = abrir(LOCK_SH);
    cargar(fd, &e);
    cerrar(fd);

    for (int i = 0; i < e.num; i++)
        suma += e.m[i].valor;

    snprintf(res, sizeof res, "fase=%d muestras=%d promedio=%.2f",
             e.fase, e.num, e.num ? suma / e.num : 0.0);
    return res;
}

const char *incrementar_fase(char *args)                /* escritura: LOCK_EX */
{
    static char res[64];
    struct Exp e;

    (void)args;
    int fd = abrir(LOCK_EX);
    cargar(fd, &e);
    e.fase++;
    guardar(fd, &e);
    cerrar(fd);

    snprintf(res, sizeof res, "%d", e.fase);
    return res;
}

const char *obtener_fase(char *args)                    /* lectura: LOCK_SH */
{
    static char res[64];
    struct Exp e;

    (void)args;
    int fd = abrir(LOCK_SH);
    cargar(fd, &e);
    cerrar(fd);

    snprintf(res, sizeof res, "%d", e.fase);
    return res;
}

struct { const char *nombre; handler_fn fn; } tabla[4] = {
    {"registrar_muestra", registrar_muestra},
    {"leer_estado",       leer_estado},
    {"incrementar_fase",  incrementar_fase},
    {"obtener_fase",      obtener_fase}
};

int main(void)
{
    struct sockaddr_in addr = {0};
    struct Exp vacio = {0};
    char linea[128], cmd[32], args[96];

    /* archivo inicial vacío (se reinicia en cada ejecución del servidor) */
    int fd0 = open(ARCHIVO, O_RDWR | O_CREAT | O_TRUNC, 0644);
    write(fd0, &vacio, sizeof vacio);
    close(fd0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8003);

    signal(SIGCHLD, SIG_IGN);                   /* evita zombis */

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(srv, (struct sockaddr *)&addr, sizeof(addr));
    listen(srv, 5);

    while (1) {
        int cli = accept(srv, NULL, NULL);

        if (fork() == 0) {                      /* hijo: atiende a este cliente */
            close(srv);
            FILE *in = fdopen(cli, "r");

            while (fgets(linea, sizeof linea, in)) {
                const char *res = NULL;
                cmd[0] = '\0';
                args[0] = '\0';

                if (sscanf(linea, "CALL %31s %95[^\n]", cmd, args) >= 1)
                    for (int i = 0; i < 4; i++)
                        if (strcmp(cmd, tabla[i].nombre) == 0)
                            res = tabla[i].fn(args);

                printf("[ILUS pid=%d] %s(%s) -> %s\n", getpid(), cmd, args, res ? res : "ERROR");

                if (res)
                    dprintf(cli, "RESULT %s\n", res);
                else
                    dprintf(cli, "ERROR\n");
            }

            fclose(in);                         /* cierra cli */
            return 0;
        }

        close(cli);                             /* padre: ya no usa este socket */
    }
}