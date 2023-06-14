//kernel/sem.c
#include <sem.h>
#include <string.h> /* strcpy()  strcmp() */
#include <asm/segment.h>  /* get_fs_byte() */
#include <unistd.h>  /* NULL */
#include <asm/system.h>  /* cli()  sti() */
#include <linux/kernel.h>  /* printk() */

#define MAX_SEM 5

static sem_t sems[MAX_SEM]={
    {"",0,NULL},
    {"",0,NULL},
    {"",0,NULL},
    {"",0,NULL},
    {"",0,NULL},
}

//创建新信号量
sem_t* sys_sem_open(const char* name, unsigned int value){
    if(name==NULL)    printk("Name is empty.\n");
    int i;
    sem_t* tmp;
    for(i=0;i<MAX_SEM;i++){
        if(strcmp(sems[i],"")!=0){
            strcpy(&sems[i].name,name);
            sems[i].value=value;
            tmp=&sems[i];
            break;
        }
    }
    return tmp;
}

//信号量P操作
int sys_sem_wait(sem_t* sem){
    cli();
    while(sem->value==0){
        sleep_on(&sem->wait_queue);
    }
    sem->value--;
    sti();
    return 0;
}

//信号量V操作
int sys_sem_post(sem_t* sem){
    cli();

    wake_up(&sem->wait_queue);
    sem->value++;

    sti();
    return 0;
}

//移除信号量
int sys_sem_unlink(const char* name){
    if(name==NULL)  printk("Name is empty.\n");
    char tmp[MAX_NAME];
    strcpy(tmp,name);
    int i;
    for(i=0;i<MAX_SEM;i++){
        if(strcmp(tmp,name)==0){
            sems[i].name[0]='\0';
            sems[i].value=0;
            sems[i].wait_queue=NULL;
        }
    }
    return 0;
}