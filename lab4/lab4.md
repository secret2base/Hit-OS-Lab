### Lab 4 基于内核栈切换的进程切换
- 将Linux 0.11中采用的TSS切换部分去掉，换成基于堆栈切换
  - 修改schedule
  - 重写switch_to，实现压栈、切换PCB、重写TSS指针、切换内核栈、切换LDT等任务
  - 修改copy_process，以实现进程切换时的内核栈切换


#### 4.1 修改schedule
在kernel/sched.c中
```c
//原schedule()函数如下
if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
    c = (*p)->counter, next = i;

//......

switch_to(next);
//其中next为下一个进程对应的下标（在task[]数组中）
```