### Lab 6 地址映射与共享

- 跟踪地址翻译过程
  - 在Linux 0.11中通过test.c跟踪逻辑地址、LDT表、GDT表等信息
- 基于共享内存的生产者-消费者程序
  - 使用共享内存实现生产者-消费者程序，且二者作为不同的程序
- 共享内存的实现
  - 在Linux 0.11中增加shmget()和shmat()两个系统调用，以支持生产者-消费者程序的运行
