#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctype.h>


#define MAGIC_NUMBER 'p'//use in ioctl (best major fixed?)
#define SET_HIGH_PR _IO(MAGIC_NUMBER,0x00)
#define SET_LOW_PR _IO(MAGIC_NUMBER,0x01)
#define SET_OP_BLOCK _IO(MAGIC_NUMBER,0x02)
#define SET_OP_NONBLOCK _IO(MAGIC_NUMBER,0x03)
#define SET_TIMEOUT_BLOCK _IOW(MAGIC_NUMBER,0x04,int32_t*)

typedef enum __command{
    HIGH = 0,
    LOW,
    BLOCK,
    NONBLOCK,
    TIMER
}command;

command get_cmd(char* conv){
    char tmp[strlen(conv)];
    for (int i = 0; conv[i] != '\0'; i++) {
        tmp[i] = tolower(conv[i]);
    }
    
    if (strcmp("high",tmp) == 0)
    {
        return HIGH;
    }
    if (strcmp("low",tmp) == 0)
    {
        return LOW;
    }
    if (strcmp("block",tmp) == 0)
    {
        return BLOCK;
    }
    if (strcmp("nonblock",tmp) == 0)
    {
        return NONBLOCK;
    }
    if (strcmp("time",tmp) == 0)
    {
        return TIMER;
    }
    if (strcmp("timer",tmp) == 0)
    {
        return TIMER;
    }
    if (strcmp("timeout",tmp) == 0)
    {
        return TIMER;
    }
    return -1;
    
}

int main(int argc, char** argv){
    int fd;
    command cmd;
    int32_t value;
    if (argc < 3)
    {
        printf("Wrong usage, try: %s dev_path command [args...] \n",argv[0]);
        return -1;

    }
    

    //open device
    fd = open(argv[1], O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file...\n");
        return -1;
    }
    cmd = get_cmd(argv[2]);
    if (cmd == -1)
    {
        printf("Wrong command use one of this list\n[high,low,block,nonblock,time]\n");
        return -2;
    }
    

    switch (cmd)
    {
    case HIGH:
        printf("Setting priority HIGH to device %s\n",argv[1]);
        ioctl(fd, SET_HIGH_PR); 
        break;
    case LOW:
        printf("Setting priority HIGH to device %s\n",argv[1]);
        ioctl(fd, SET_LOW_PR); 
        break;
    case BLOCK:
        printf("Setting operation blocking to device %s\n",argv[1]);
        ioctl(fd, SET_OP_BLOCK); 
        break;
    case NONBLOCK:
        printf("Setting operation non blocking to device %s\n",argv[1]);
        ioctl(fd, SET_OP_NONBLOCK); 
        break;
    case TIMER:
        value = atoi(argv[3]);
        printf("Setting timeout for device %s to value %d\n",argv[1],value);
        ioctl(fd, SET_TIMEOUT_BLOCK,(int32_t*) &value); 
        break;
    
    default:
        printf("Command error\n");
        break;
    }
}

