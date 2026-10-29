/* mutex_lamport.c — esqueleto */
typedef enum { REQUEST, REPLY, RELEASE } MsgType;
typedef struct { MsgType type; int ts; int pid; } Msg;

int ch[NPROC][NPROC][2];   /* ch[i][j]: pipe por el que i le escribe a j */

void enviar(int yo, int a, MsgType t, int ts) {
    Msg m = { t, ts, yo };
    write(ch[yo][a][1], &m, sizeof m);
}

void proceso(int yo) {
    int reloj = 0, replies = 0, recibidos = 0, entre = 0;
    int req[NPROC] = { -1, -1, -1 };   /* REQUEST pendiente de cada proceso */

    usleep(yo * 1000);

    /* 1. REQUEST a todos */
    req[yo] = ++reloj;
    for (int j = 0; j < NPROC; j++)
        if (j != yo) enviar(yo, j, REQUEST, req[yo]);

    while (!entre || recibidos < 6) {

        /* 3. entrar si tengo todos los REPLY y mi request es el menor */
        if (!entre && replies == 2) {
            int menor = 1;
            for (int j = 0; j < NPROC; j++)
                if (j != yo && req[j] != -1 &&
                    (req[j] < req[yo] || (req[j] == req[yo] && j < yo)))
                    menor = 0;
            if (menor) {
                FILE *f = fopen("log.txt", "a");
                fprintf(f, "P%d entra (ts=%d)\n", yo, req[yo]);
                fflush(f);
                usleep(50000);
                fprintf(f, "P%d sale\n", yo);
                fclose(f);

                /* 4. RELEASE a todos */
                req[yo] = -1;
                reloj++;
                for (int j = 0; j < NPROC; j++)
                    if (j != yo) enviar(yo, j, RELEASE, reloj);
                entre = 1;
                continue;
            }
        }

        /* esperar mensajes de los otros dos */
        struct pollfd fds[2];
        int de[2], k = 0;
        for (int j = 0; j < NPROC; j++)
            if (j != yo) {
                fds[k].fd = ch[j][yo][0];
                fds[k].events = POLLIN;
                de[k++] = j;
            }
        poll(fds, 2, -1);

        for (int i = 0; i < 2; i++) {
            if (!(fds[i].revents & POLLIN)) continue;
            Msg m;
            read(fds[i].fd, &m, sizeof m);
            recibidos++;
            if (m.ts > reloj) reloj = m.ts;   /* regla Lamport */
            reloj++;

            if (m.type == REQUEST) {          /* 2. responder REPLY */
                req[m.pid] = m.ts;
                reloj++;
                enviar(yo, m.pid, REPLY, reloj);
            } else if (m.type == REPLY) {
                replies++;
            } else {                          /* RELEASE */
                req[m.pid] = -1;
            }
        }
    }
    _exit(0);
}

int main(void) {
    for (int a = 0; a < NPROC; a++)
        for (int b = 0; b < NPROC; b++)
            if (a != b) pipe(ch[a][b]);

    for (int i = 0; i < NPROC; i++)
        if (fork() == 0) proceso(i);

    for (int i = 0; i < NPROC; i++) wait(NULL);
    return 0;
}