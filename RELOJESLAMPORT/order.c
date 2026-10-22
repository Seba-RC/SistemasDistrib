/* order.c */ 

#include <stdio.h> 

#include <stdlib.h> 

#include <string.h> 

 

typedef struct { 

    char proc[4]; 

    int  ts; 

    char name[8]; 

} Event; 

 

int total_order_cmp(const void *a, const void *b) { 
    const Event *ea = (const Event *)a;
    const Event *eb = (const Event *)b;

    if (ea->ts != eb->ts) 
        return (ea -> ts > eb -> ts) - (ea -> ts < eb -> ts); /* retorna 1 si ea-> ts mayor, 0 si ea -> ts menor*/
        /*
         1 - 0 = 1 si ea es mayor
         0 - 1 = -1 si ea es menor
        */
    return strcmp(ea->proc, eb->proc);
} 

 

Event events[] = { 

    {"P2",4,"α"}, {"P1",4,"β"}, {"P3",1,"γ"}, {"P1",7,"δ"}, 

    {"P3",4,"ε"}, {"P2",9,"ζ"}, {"P1",2,"η"}, {"P3",6,"θ"}, 

}; 

#define N (sizeof(events)/sizeof(events[0])) 

 

int main(void) { 

    qsort(events, N, sizeof(Event), total_order_cmp); 

    for (int i = 0; i < (int)N; i++) 

        printf("%d. [%s ts=%d] %s\n", i+1, events[i].proc, events[i].ts, events[i].name); 

    return 0; 

} 