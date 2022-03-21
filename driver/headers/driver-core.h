

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

#include "defines.h"
#include "klist.h"



#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
#define get_major(session)	MAJOR(session->f_inode->i_rdev)
#define get_minor(session)	MINOR(session->f_inode->i_rdev)
#else
#define get_major(session)	MAJOR(session->f_dentry->d_inode->i_rdev)
#define get_minor(session)	MINOR(session->f_dentry->d_inode->i_rdev)
#endif

#define WQ_STR_LEN 4

enum priority {
    HIGH_PR = 0,
    LOW_PR
};

#define DEFAULT_PR HIGH_PR
#define PRIORITY_NUM 2

enum states{
    ENABLED = 0,
    DISABLED
};

enum io_cmd {
    SET_HIGH_PR = 0,
    SET_LOW_PR,
    SET_OP_BLOCK,
    SET_OP_NONBLOCK,
    SET_TIMER_ON_BLOCK
};

typedef struct _session_state{
    int priority;
    bool blocking;
    //TODO timer setup 
    unsigned long timer;
} session_state;


typedef struct _device_state{
    int state; //ENABLE OR
    unsigned int thread_wait[PRIORITY_NUM];
    klist* data_flow[PRIORITY_NUM];
    struct workqueue_struct* wq; 
} device_state;



/**
 * Deferred work structers
*/
typedef struct _packed_work{
        char* buffer;
        int len;
        device_state* device;
        struct work_struct the_work;
} packed_work;

extern int deferred_put(char*,int,device_state*,struct workqueue_struct *);
extern void actual_work(unsigned long);