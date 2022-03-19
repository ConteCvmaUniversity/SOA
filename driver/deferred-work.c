#include "headers/deferred-work.h"

void actual_work(unsigned long data){
    int byte,minor;
    device_state device;
    packed_work* work;

    work = (packed_work*) (container_of((void*)data,packed_work,the_work));

    minor = work->minor;
    device = devices[minor];


    byte = klist_put(device.data_flow[LOW_PR],work->buffer,work->len);
    if (byte < 0)
    {
        //Kernel kmalloc no mem 
    }

    kfree(work);
    return;   
    
}

int deferred_put(char* buf,int len,int minor,struct workqueue_struct *queue){
    packed_work *task;

    

    task = kmalloc(sizeof(packed_work),GFP_KERNEL);
    if (!task)
    {
        printk("tasklet buffer allocation failure\n");
        return -ENOMEM;
    }
    task->len = len;
    task->buffer = buf;
    task->minor = minor;
    __INIT_WORK(&(task->the_work),(void*)actual_work,(unsigned long)(&(task->the_work)));
    queue_work(queue,&(task->the_work));
    
    return 0;
}