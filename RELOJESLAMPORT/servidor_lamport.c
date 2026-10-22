#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct { 
    int  ts; 
    char payload[64]; 
} Msg; 

static int global_clock = 0; 

void handle_client(int fd) { 

    Msg msg, resp; 

    while (recv(fd, &msg, sizeof(msg), 0) > 0) { 
        /* 1. Actualizar reloj al recibir */ 
        global_clock = (msg.ts > global_clock ? msg.ts : global_clock) + 1;  /* msg.ts es mayorq ue global clock? Si lo es, entonces usamos msg.ts; si no, global_clock*/

        printf("[srv clock=%d] recibido '%s'\n", global_clock, msg.payload); 

        /* 2. Evento interno de procesamiento */ 
        global_clock++;

        /* 3. Preparar respuesta con timestamp */ 
        snprintf(resp.payload, sizeof(resp.payload), "ACK"); 
        global_clock++;
        resp.ts = global_clock;


        send(fd, &resp, sizeof(resp), 0); 

    } 

} 

/* Fragmento del cliente */ 

void client_send(int fd, int *clock, const char *text) { 

    Msg msg; 

    /* TODO: incrementar reloj, adjuntar timestamp, enviar */ 
    (*clock)++;
    msg.ts = *clock;
    snprintf(msg.payload, sizeof(msg.payload), "%s", text); 

    send(fd, &msg, sizeof(msg), 0); 

} 