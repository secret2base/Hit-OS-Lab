### Lab 5 信号量的实现和应用

- 在Ubuntu下编写程序，用信号量解决生产者消费者问题
  - 建立一个生产者进程，N个消费者进程
  - 用文件建立一个共享缓冲区
  - 生产者进程依次向缓冲区写入正数0-M(M>=500)
  - 消费者进程从缓冲区读数，每次读一个，并将读出的数字从缓冲区删除，然后将本进程ID和数字输出到标准输出
  - 缓冲区同时最多保存10个数
  - 
- 在0.11中实现信号量，并用生产者消费者程序检验


#### 5.1 编写生产者消费者程序
```c
sem_t *sem_open(const char * name,unsigned int value);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_unlink(const char * name);
```
sem_open()创建一个信号量，或打开一个已经存在的信号量
- sem_t是信号量类型
- name是信号量名字
- value是信号量的初值，仅当新建信号量时该参数有效

sem_wait()信号量的P操作
sem_post()信号量的V操作
sem_unlink()删除信号量

kernel/sem.c中实现

gcc -pthread -o pc pc.c
sem_t* empty = sem_open("Empty",O_CREAT, S_IRUSR | S_IWUSR, 10);
注意返回值为指针

程序结束但信号量未释放会影响到下一次执行

pc.c还是有一些问题要改
读出来的数字是乱的，因为生产者从后面写，消费者也从后面读，改成生产者后面写消费者前面读，读完把后面的往前移动  

等待所有子进程结束  
while (waitpid(-1, NULL, 0) > 0);


2  
按要求编写 sem.c sem.h
添加系统调用 在include/linux/sys.h 中增加， 在include/unistd.h中增加  
ftruncate的替代，修改fcntl.h
怎么有好几个linux文件夹
修改Makefile