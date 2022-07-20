#include "headers/klist.h"


//Allocation of klist
klist* klist_alloc(void){
    klist* list;
    list = kmalloc(sizeof(klist),GFP_KERNEL);
    if (!list)
    	return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    list->reserved = 0;
    mutex_init(&(list->op_mtx));
    
    return list;
}

klist_elem* klist_elem_alloc(char* buffer,int size,gfp_t flags){
    klist_elem* elem;
    elem = kmalloc(sizeof(klist),flags);
    if (!elem)
    	return NULL;
    elem->last_read = 0;
    elem->next= NULL;
    elem->size = size;
    elem->buffer = buffer;

    return elem;
}

//This function is call only in already lock zone
void remove_head(klist* list){
    klist_elem* elem;
    elem = list->head;
    list->head = elem->next;
    if (list->head == NULL)
    {
        list->tail = NULL;
    }
    klist_elem_free(elem);
    return;
}

bool reserve_space(klist* list,unsigned long space){
    unsigned long tmp;
    bool ret;
    mutex_lock(&(list->op_mtx));
    tmp = list->reserved + space;
    if (tmp > KLIST_MAX_SIZE)
    {
        ret = false;
    } else
    {
        list->reserved = tmp;
        ret = true;
    } 
    mutex_unlock(&(list->op_mtx));
    return ret;
}

void free_reserved_space(klist* list,unsigned long space){
    mutex_lock(&(list->op_mtx));
    list->reserved -= space;
    mutex_unlock(&(list->op_mtx));
    //TODO consistency check
    return;
}

int klist_put(klist* list,char* buffer,unsigned int size,gfp_t flags){
    //create an list element 
    klist_elem* elem;
    
    
    elem = klist_elem_alloc(buffer,size,flags);
    
    
    if (elem == NULL)
        return -ENOMEM;
       
    mutex_lock(&(list->op_mtx));

    if (list->tail != NULL)
    {
        list->tail->next=elem;
        list->tail = elem;
    }else
    {
        //no element in list
        list->head = elem;
        list->tail = elem;
        
    }
    

    list->len += size;
    mutex_unlock(&(list->op_mtx));

    return size;
    
}

int klist_get(klist* list,char* buffer,unsigned int size){
    klist_elem* elem;
    unsigned int total, remaining,byte_to_read;

    if (list->len == 0) {
        return -ENODATA;
    }
    
    mutex_lock(&(list->op_mtx));
    
    total = min(list->len,(unsigned long)size);  //because get can request more byte of those are in the struct
    remaining = 0;
    while (remaining != total)
    {
        elem = list->head;
        byte_to_read = total - remaining;
        
        if (byte_to_read < (elem->size-elem->last_read))
        {
            //no del haed
            memcpy(buffer+remaining, elem->buffer+elem->last_read ,byte_to_read);
            elem->last_read += byte_to_read;
            remaining += byte_to_read; 
            if (elem->last_read >= elem->size)
            {
                remove_head(list);
            }
            
        }else
        {
            //read all elem->buffer and remove head
            memcpy(buffer+remaining, elem->buffer ,elem->size);
            remove_head(list);
            remaining += elem->size;
        }
    }
    list->len -= total;
    mutex_unlock(&(list->op_mtx));
    return total;
}

unsigned long klist_len(klist* list){
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
        while (list->head != NULL)
        {
            remove_head(list);
        }
        mutex_unlock(&(list->op_mtx));
    }
    
    kfree(list);
   
    return;
}