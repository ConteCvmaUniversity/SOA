#include <linux/mutex.h>


#include "defines.h"

/* klist metadata fifo rule */


typedef struct _klist_elem{
    char* buffer;
    int last_read;
    int size;
    struct _klist_elem* next; 
}klist_elem;


typedef struct _klist
{
    struct mutex op_mtx;
    klist_elem* head;
    klist_elem* tail;
    unsigned int len;

}klist;

/*Function*/

extern klist* klist_alloc(void);
extern void klist_free(klist*);
extern int klist_put(klist*,char*,unsigned int,gfp_t);
extern int klist_get(klist*,char*,unsigned int);
extern unsigned int klist_len(klist*);

klist_elem* klist_elem_alloc(char*,int,gfp_t);
void klist_elem_free(klist_elem*);
