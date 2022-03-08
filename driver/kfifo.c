#include "kfifo.h"


kfifo* fifo_alloc(){
    kfifo* fifo;
    fifo = kmalloc(sizeof(kfifo), GFP_KERNEL);
    if (!fifo)
    	return NULL;
    fifo->buffer = (char*)__get_free_page(GFP_KERNEL); //the kfifo size is PAGESIZE ...can be parametrized (kmalloc)
    if (!fifo->buffer)
        return NULL;
    
    fifo->head = fifo->tail = 0;
	fifo->size = PAGESIZE;
	fifo->len = 0;
    mutex_init(&(fifo->op_mtx));

    return fifo;
}

void fifo_free(kfifo* fifo){
    free_page((unsigned long) fifo->buffer);
    kfree(fifo);
}

void fifo_reset(kfifo* fifo){
    mutex_lock(&(fifo->op_mtx));
    fifo->head = fifo->tail = 0;
	fifo->len = 0;
    mutex_unlock(&(fifo->op_mtx));
}

unsigned int fifo_put(kfifo* fifo,char* buffer,unsigned int count){
    unsigned int total, remaining;
    mutex_lock(&(fifo->op_mtx));
    total = remaining = min(count, fifo->size - fifo->len); //can put if fifo have space 
	while (remaining > 0) {
		unsigned int l = min(remaining, fifo->size - fifo->tail); //need to cycle on buffer if there is space 
		memcpy(fifo->buffer + fifo->tail, buffer, l);
		fifo->tail += l;
		fifo->tail %= fifo->size;
		fifo->len += l;
		buffer += l;
		remaining -= l;
	}

    mutex_unlock(&(fifo->op_mtx));
    return total;
}


//TODO verificare il buffer passato 
unsigned int fifo_get(kfifo* fifo,char* buffer,unsigned int count){
    unsigned int total, remaining;
    mutex_lock(&(fifo->op_mtx));
    total = remaining = min(count, fifo->len);
	while (remaining > 0) {
		unsigned int l = min(remaining, fifo->size - fifo->head);
		memcpy(buffer, fifo->buffer + fifo->head, l);
		fifo->head += l;
		fifo->head %= fifo->size;
		fifo->len -= l;
		buffer += l;
		remaining -= l;
	}

    mutex_unlock(&(fifo->op_mtx));
    return total;
}

unsigned int fifo_len(kfifo* fifo){
    unsigned int result;
    mutex_lock(&(fifo->op_mtx));
    result = fifo->len;
    mutex_unlock(&(fifo->op_mtx));
    return result;
}