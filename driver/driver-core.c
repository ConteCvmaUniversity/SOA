/**
 * Main core of project driver
 * 
*/

#define EXPORT_SYMTAB


#include "defines.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marco Calavaro");

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static ssize_t dev_read(struct file *filp, char *buff, size_t len, loff_t *off);
static long dev_ioctl(struct file *filp, unsigned int command, unsigned long param);


/**
 * Global module variable
*/
#define MODULE_NAME "project-module"
static int Major; // Major number of driver
device_state devices[MINORS]; // array that mantain state of all device
#define DEVICE_NAME "project-dev"

/**
 * Global operation
*/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
#define get_major(session)	MAJOR(session->f_inode->i_rdev)
#define get_minor(session)	MINOR(session->f_inode->i_rdev)
#else
#define get_major(session)	MAJOR(session->f_dentry->d_inode->i_rdev)
#define get_minor(session)	MINOR(session->f_dentry->d_inode->i_rdev)
#endif

/*Default session parameter*/
void setup_session_state(session_state* state,unsigned int flags){
    if (flags & O_NONBLOCK){
        state->blocking = false;
    } else
    {
        state->blocking = true;
    }

    //set priority to default?
    state->priority = DEFAULT_PR;
    state->timer = TIMEOUT_BLOCKING_DEFAULT;
}

/**
 * Driver operation
*/
static int dev_open(struct inode *inode, struct file *file) {
    session_state* state;
    int minor = get_minor(file);
    if(minor >= MINORS){
        return -ENODEV;
    }
    if (devices[minor].state == DISABLED)
        return -DISABLED;

    state = kmalloc(sizeof(session_state),GFP_KERNEL);
    if (state == NULL)
        return -ENOMEM;
    
    setup_session_state(state,file->f_flags);
    file->private_data = (void*) state; 

    

    printk(KERN_INFO "%s: device file successfully opened with minor %d\n",MODULE_NAME,minor);
    return 0;
    
}

static int dev_release(struct inode *inode, struct file *file) {
    int minor = get_minor(file);
    

    //free private data file zone
    kfree(file->private_data);

    printk(KERN_INFO "%s: device file whit minor %d closed\n",MODULE_NAME,minor);
    return 0;

}

static ssize_t dev_write(struct file *filp, const char *buff, size_t len, loff_t *off) {
    int minor = get_minor(filp);
    session_state* session;
    device_state device;
    unsigned int ret;

    session = (session_state*) filp->private_data;
    device = devices[minor];

    
    //check priority
    if (session->priority == HIGH_PR)
    {
        
        char* tmp = kmalloc(len,GFP_KERNEL);
        ret = copy_from_user(tmp,buff,len);
        if (ret != 0)
        {
            //some error 
            kfree(tmp);
            return -1; //TODO
        }
        
        ret = fifo_put(device.data_flow[HIGH_PR],tmp,len);
        kfree(tmp);
    }else
    {
       //deferred work 


    }
    
    




    return ret;
}

static ssize_t dev_read(struct file *filp, char *buff, size_t len, loff_t *off) {
    int minor = get_minor(filp);
    session_state* session;
    device_state device;
    unsigned int ret;

    session = (session_state*) filp->private_data;
    device = devices[minor];

    char* tmp = kmalloc(len,GFP_KERNEL);
    ret = fifo_get(device.data_flow[session->priority],tmp,len);
    if (ret==0)
    {
        //no data todo
    }

    ret = copy_to_user(buff,tmp,len);
    if (ret != 0)
    {
        //some error 
        kfree(tmp);
        return -1; //TODO
    }
    

    kfree(tmp);
    return ret;
}

static long dev_ioctl(struct file *filp, unsigned int command, unsigned long param) {
    int minor = get_minor(filp);
    session_state* state;
    state = (session_state*) filp->private_data;

    printk(KERN_INFO "%s: call ioctl on device %d (minor number) and command %u\n",MODULE_NAME,minor,command);

    switch (command)
    {
    case SET_HIGH_PR:
        state->priority = HIGH_PR;
        break;
    case SET_LOW_PR:
        state->priority = LOW_PR;
        break;
    case SET_OP_BLOCK:
        state->blocking = true;
        break;
    case SET_OP_NONBLOCK:
        state->blocking = false;
        break;
    case SET_TIMER_ON_BLOCK:
        state->timer = param;
        break;

    default:
        printk(KERN_INFO "%s: call ioctl whit unknow command on device %d (minor number)\n",MODULE_NAME,minor);
        break;
    }

    return 0;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .write = dev_write,
  .read = dev_read,
  .open =  dev_open,
  .release = dev_release,
  .unlocked_ioctl = dev_ioctl
};


/**
 * Module operations
*/

int init_module(void){
    //startup devices
    int i;
    device_state tmp;
    for(i=0 ; i<MINORS ; i++){
        tmp = devices[i];     
        tmp.state = ENABLED; //default prority set to LOW
        tmp.thread_wait_high = 0;
        tmp.thread_wait_low = 0;
        tmp.data_flow[HIGH_PR] = klist_alloc();
        if(tmp.data_flow[HIGH_PR] == NULL ) goto revert_alloc; //if some error on get_free_page
        tmp.data_flow[LOW_PR] = klist_alloc();
        if(tmp.data_flow[LOW_PR] == NULL ) goto revert_alloc; //if some error on get_free_page
    }
    //no error
    //dinamic major
    Major = __register_chrdev(0, 0, MINORS, DEVICE_NAME, &fops);
    if (Major < 0)
    {
        printk("%s: char device registration failed\n",MODULE_NAME);
        return Major;
    }
    
    printk(KERN_INFO "%s: new char device registered, it is assigned major number %d\n",MODULE_NAME, Major);
    return 0;

    //error
    revert_alloc:
    for(;i>=0;i--){
        klist_free(devices[i].data_flow[HIGH_PR]);
        klist_free(devices[i].data_flow[LOW_PR]);
	}
	return -ENOMEM;
}

void cleanup_module(void){
    int i;
	for(i=0;i<MINORS;i++){
		klist_free(devices[i].data_flow[HIGH_PR]);
        klist_free(devices[i].data_flow[LOW_PR]);
	}

	unregister_chrdev(Major, DEVICE_NAME);

	printk(KERN_INFO "%s: new device unregistered, it was assigned major number %d\n",MODULE_NAME, Major);

	return;
}