
#include "defines.h"
#include "driver-core.h"

typedef struct _packed_work{
        char* buffer;
        int len;
        int minor;
        struct work_struct the_work;
} packed_work;

extern int deferred_put(char*,int,int,struct workqueue_struct *);
extern void actual_work(unsigned long);