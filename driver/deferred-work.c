#include "headers/driver-core.h"

void actual_work(unsigned long data){
    int byte;
    device_state* device;
    packed_work* work;
    

    work = (packed_work*) (container_of((void*)data,packed_work,the_work));    
    device = work->device;
    
    //mutex_lock(&(device->data_flow[LOW_PR]->op_mtx));
    byte = klist_put(device->data_flow[LOW_PR],work->buffer,work->len,GFP_KERNEL);
    low_prio_data[device->id] = klist_len(device->data_flow[LOW_PR]);
    //mutex_unlock(&(device->data_flow[LOW_PR]->op_mtx));
    wake_up_interruptible(&(device->waitq[LOW_PR]));


    if (byte < 0)
    {
        //Kernel kmalloc no mem  TODO??
    }

    kfree(work);
    module_put(THIS_MODULE);
    return;   
    
}

int deferred_put(char* buf,int len,device_state* device,struct workqueue_struct *queue){
    packed_work *task;
    if(!try_module_get(THIS_MODULE)) return -ENODEV;

    task = kmalloc(sizeof(packed_work),GFP_KERNEL);
    if (!task)
    {
        printk("tasklet buffer allocation failure\n");
        return -ENOMEM;
    }
    task->len = len;
    task->buffer = buf;
    task->device = device;
    __INIT_WORK(&(task->the_work),(void*)actual_work,(unsigned long)(&(task->the_work)));
    if(!queue_work(queue,&(task->the_work))) return -ENOMEM;
    
    return 0;
}