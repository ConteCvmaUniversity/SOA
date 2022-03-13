#include "deferred-work.h"

void actual_work(unsigned long data){

}

void deferred_put(char* buf,int minor,struct workqueue_struct *queue){
    packed_work *task;

    task = kmalloc(sizeof(packed_work),GFP_KERNEL);
    if (!task)
    {
        printk("tasklet buffer allocation failure\n");
        return -ENOMEM;
    }
    task->buffer = buf;
    task->minor = minor;
    __INIT_WORK(&(task->the_work),(void*)actual_work,(unsigned long)(&(task->the_work)));
    queue_work(queue,&(task->the_work));

}