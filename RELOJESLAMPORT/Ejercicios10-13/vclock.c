#include <stdio.h>
#include <string.h>

#define NPROC 3

typedef struct {
    int v[NPROC];  /* v[i] = conocimiento sobre Pi */
    int pid;
} VNode;

void vc_local(VNode *n) {
    n->v[n->pid]++;
}

void vc_send(VNode *n, int out[NPROC]) {
    n->v[n->pid]++; /* evento de envio */
    memcpy(out, n->v, sizeof(int) * NPROC); 
    /* 
    destino: out, arreglo del mensaje
    origen: n->v
    cantidad de bytes: sizeof(int) * cantidad de componentes
    */
}

void vc_recv(VNode *n, const int rcv[NPROC]) {
    /* TODO: máximo componente a componente, luego incrementa */
    for (int j=0; j < NPROC; j++)
        if (rcv[j] > n->v[j])
            n->v[j] = rcv[j]; /* compara maximo componente a componente */
    n->v[n->pid]++; /* incrementa la componente propia */
}

/* a → b  sii  ∀i: a[i] ≤ b[i]  y  ∃j: a[j] < b[j] */
int vc_happens_before(const int a[NPROC], const int b[NPROC]) {
    /* TODO */
    int estricto = 0;
    for (int i = 0; i < NPROC; i++) {
        if (a[i] > b[i]) return 0;
        if (a[i] < b[i]) estricto = 1;
    }
    return estricto;
}

int vc_concurrent(const int a[NPROC], const int b[NPROC]) {
    int vc_concurrent(const int a[NPROC], const int b[NPROC]) {
    return !vc_happens_before(a, b) && !vc_happens_before(b, a)
           && memcmp(a, b, sizeof(int) * NPROC) != 0;
}
}

void print_vc(const char *label, const int v[NPROC]) {
    printf("%s: [%d,%d,%d]\n", label, v[0], v[1], v[2]);
}

int main(void) {
    VNode p0 = {{0,0,0}, 0};
    VNode p1 = {{0,0,0}, 1};
    VNode p2 = {{0,0,0}, 2};
    int ts[NPROC], snap_a[NPROC], snap_b[NPROC];

    vc_local(&p0);                       /* evento a */
    memcpy(snap_a, p0.v, sizeof(ts));
    print_vc("P0 tras a", p0.v);

    vc_local(&p2);                       /* evento b — independiente */
    memcpy(snap_b, p2.v, sizeof(ts));
    print_vc("P2 tras b", p2.v);

    vc_send(&p0, ts);                    /* P0 envía m1 */
    vc_recv(&p1, ts);                    /* P1 recibe m1 */
    print_vc("P1 tras recv m1", p1.v);

    printf("a → P1_actual: %s\n",
           vc_happens_before(snap_a, p1.v) ? "SÍ" : "NO");
    printf("a ∥ b:         %s\n",
           vc_concurrent(snap_a, snap_b) ? "SÍ" : "NO");
    return 0;
}
