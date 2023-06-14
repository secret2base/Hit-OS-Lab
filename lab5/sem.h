#include<linux/sched.h>

#define MAX_NAME 20

typedef struct sem_t{
    char name[MAX_NAME];
    unsigned int value;
    struct task_struct *wait_queue;
}sem_t;