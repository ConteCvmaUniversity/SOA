#include <linux/mutex.h>
#include <linux/slab.h>

#define PAGESIZE 4096

//circular buffer
typedef struct _fifo{
    struct mutex op_mtx;
    unsigned int head;
    unsigned int tail;
    unsigned int size; //size of buffer
    unsigned int len; //byte written in the buffer
    char *buffer;
} kfifo;

extern kfifo* fifo_alloc(void);
extern void fifo_free(kfifo*);
extern void fifo_reset(kfifo*);
extern unsigned int fifo_put(kfifo*,char*,unsigned int);
extern unsigned int fifo_get(kfifo*,char*,unsigned int);
extern unsigned int fifo_len(kfifo*);