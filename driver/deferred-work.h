#include <workqueue.h>
#include "defines.h"

typedef struct _packed_work{
        char* buffer;
        int minor;
        struct work_struct the_work;
} packed_work;

extern void deferred_put(char*,int,struct workqueue_struct *);
extern void actual_work(unsigned long);