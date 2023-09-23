#ifndef _SEM_H
#define _SEM_H
 
#include <linux/sched.h>
 
#define SEMTABLE_LEN    20
#define SEM_NAME_LEN    20
 
typedef struct semaphore
{
    char name[SEM_NAME_LEN];    /* 信号量名称 */
    int value;                  /* 信号量值 */
    struct task_struct *queue;  /* 信号量等待队列 */
} sem_t;
 
extern sem_t semtable[SEMTABLE_LEN];  /* 定义一个信号量表 */
 
#endif