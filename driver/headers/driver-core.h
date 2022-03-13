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
#include "deferred-work.h"


enum priority {
    HIGH_PR = 0,
    LOW_PR
};

#define DEFAULT_PR LOW_PR

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
    int state;
    unsigned int thread_wait_high;
    unsigned int thread_wait_low;
    klist* data_flow[2]; 
} device_state;