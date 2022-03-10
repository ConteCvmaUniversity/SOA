#include "klist.h"


//Allocation of klist
klist* klist_alloc(void){
    klist* list;
    list = kmalloc(sizeof(klist),GFP_KERNEL);
    if (!list)
    	return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    mutex_init(&(list->op_mtx));

    return list;
}

klist_elem* klist_elem_alloc(char* buffer,int size){
    klist_elem* elem;
    elem = kmalloc(sizeof(klist),GFP_KERNEL);
    if (!elem)
    	return NULL;
    elem->last_read = 0;
    elem->next= NULL;
    elem->size = size;
    elem->buffer = buffer;
    return elem;
}

unsigned int klist_put(klist* list,char* buffer,unsigned int size){
    //create an list element 
    klist_elem* elem;
    elem = klist_elem_alloc(buffer,size);
    if (elem == NULL)
        return -ENOMEM;
    
    mutex_lock(&(list->op_mtx));
    if (list->head == NULL)
    {
        //no element in list
        list->head = elem;
        
    }else if (list->tail == NULL)
    {
        //only an element
        list->head->next = elem;
        list->tail = elem;

    }else
    {
       // there are alement in list
        list->tail->next = elem;
        list->tail = elem;
    }
    list->len += size;
    mutex_unlock(&(list->op_mtx));
    return size;
    
}

unsigned int klist_get(klist* list,char* buffer,unsigned int size){
    klist_elem* elem;
    unsigned int total, remaining,byte_to_read;
    
    mutex_lock(&(list->op_mtx));
    total = min(list->len,size);  //because get can request more byte of those are in the struct
    remaining = 0;
    while (remaining != total)
    {
        elem = list->head;
        byte_to_read = total - remaining;
        
        if (byte_to_read < elem->size)
        {
            //no del haed
            memcpy(buffer+remaining, elem->buffer ,byte_to_read);
            remaining += byte_to_read; 
        }else
        {
            //read all elem->buffer and remove head
            memcpy(buffer+remaining, elem->buffer ,elem->size);
            remove_head(list);
            remaining += elem->size;
        }
    }
    mutex_unlock(&(list->op_mtx));
    return total;
}


//This function is call only in already lock zone
void remove_head(klist* list){
    klist_elem* elem;
    elem = list->head;
    list->head = elem->next;
    klist_elem_free(elem);
    return;
}

unsigned int klist_len(klist* list){
    unsigned int len;
    mutex_lock(&(list->op_mtx));
    len = list->len;
    mutex_unlock(&(list->op_mtx));
    return len;
}

void klist_elem_free(klist_elem* elem){
    kfree(elem->buffer);
    kfree(elem);
    return;
}

void klist_free(klist* list){
    //if element in list free element
    if (klist_len(list)>0)
    {
        mutex_lock(&(list->op_mtx));
        while (klist->head != NULL)
        {
            remove_head(list);
        }
        mutex_unlock(&(list->op_mtx));
    }
    
    kfree(list);
   
    return;
}