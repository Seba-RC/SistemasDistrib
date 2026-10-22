#include <stdio.h>
typedef struct {
    int clock;
    char id[8];
    } Node;
void lamport_local(Node *n) {
n->clock++;
}
void lamport_send(Node *n, int *ts) {
/* TODO: incrementar y guardar en *ts el timestamp a enviar */
n->clock++;
*ts = n->clock;
}
void lamport_recv(Node *n, int ts_recv) {
n->clock = (ts_recv > n->clock ? ts_recv : n-> clock) + 1;
}
int main(void) {
    Node p1 = { 0, "P1" };
    Node p2 = { 0, "P2" };
    int ts;
    lamport_local(&p1); /* a: evento interno P1 */
    lamport_send(&p1, &ts); /* b: P1 envía m1 */
    printf("%s envía m1 ts=%d\n", p1.id, ts);
    lamport_local(&p2); /* c: evento interno P2 */
    lamport_recv(&p2, ts); /* d: P2 recibe m1 */
    printf("%s recibe m1 clock=%d\n", p2.id, p2.clock);
    lamport_send(&p2, &ts); /* e: P2 envía m2 */
    lamport_recv(&p1, ts); /* f: P1 recibe m2 */
    printf("%s recibe m2 clock=%d\n", p1.id, p1.clock);
    return 0;
}