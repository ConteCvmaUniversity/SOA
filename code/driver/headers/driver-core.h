

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/pid.h>		
#include <linux/tty.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <linux/ioctl.h>

#include "defines.h"
#include "klist.h"



#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
#define get_major(session)	MAJOR(session->f_inode->i_rdev)
#define get_minor(session)	MINOR(session->f_inode->i_rdev)
#else
#define get_major(session)	MAJOR(session->f_dentry->d_inode->i_rdev)
#define get_minor(session)	MINOR(session->f_dentry->d_inode->i_rdev)
#endif

#define WORKQ_STR_LEN 4

enum priority {
    HIGH_PR = 0,
    LOW_PR
};

#define DEFAULT_PR HIGH_PR
#define PRIORITY_NUM 2

enum states{
    DISABLED = 0,
    ENABLED     
};

#define MAGIC_NUMBER 'p'//use in ioctl (best major fixed?)
#define SET_HIGH_PR _IO(MAGIC_NUMBER,0x00)
#define SET_LOW_PR _IO(MAGIC_NUMBER,0x01)
#define SET_OP_BLOCK _IO(MAGIC_NUMBER,0x02)
#define SET_OP_NONBLOCK _IO(MAGIC_NUMBER,0x03)
#define SET_TIMEOUT_BLOCK _IOW(MAGIC_NUMBER,0x04,int32_t*)

typedef struct _session_state{
    int priority;
    bool blocking;
    //TODO timer setup 
    unsigned long timeout;
} session_state;


typedef struct _device_state{
    int id;
    //atomic_t thread_wait[PRIORITY_NUM];
    klist* data_flow[PRIORITY_NUM];
    wait_queue_head_t waitq[PRIORITY_NUM];
    struct workqueue_struct* workq; //only for low priority
} device_state;

#define blocking_lock_mutex(block,mutex_pointer) \
({ \
    int r=0; \
    if (block){\
        mutex_lock(mutex_pointer);\
    }else{\
        if(!mutex_trylock(mutex_pointer))\
            r = -EAGAIN;\
    }\
    r;\
})

/* Condition of wait event are:
            1: mutex_trylock success
            2: data on queue available
    Attention if mutex_trylock success but no data available must unlock mutex
*/
/*
#define get_wait_conditions(mutex_pointer,expression)\
({ \
    int r = false; \
    if(mutex_trylock(mutex_pointer)){ \
        if(expression) \
            r = true; \
        else \
            mutex_unlock(mutex_pointer);\
    } \
    r; \
})
*/


/**
 * Deferred work structers
*/
typedef struct _packed_work{
        char* buffer;
        int len;
        device_state* device;
        struct work_struct the_work;
} packed_work;

extern unsigned long low_prio_data[MINORS];
extern int deferred_put(char*,int,device_state*,struct workqueue_struct *);
extern void actual_work(unsigned long);