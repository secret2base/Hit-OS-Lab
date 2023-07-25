### Lab 3 进程运行轨迹的跟踪与统计
- 基于模板process.c编写多进程的样本程序，实现如下功能
  - 所有子进程都并行运行，每个子进程的实际运行时间一般不超过30秒
  - 父进程向标准输出打印所有子进程的id，并在所有子进程都退出后才退出

- 在Linux 0.11上实现进程运行轨迹的跟踪
  - 基本任务是在内核中维护一个日志文件/var/process.log，把从操作系统启动到系统关机过程中的所有进程的运行轨迹都记录在log文件中
  - 在修改过的0.11上运行页样本程序，通过分析log文件，统计该程序建立的所有进程的等待时间、完成时间（周转时间）和运行时间，也可以使用python脚本 stat_log.py （在\home\teacher\目录下）进行统计
  - 修改0.11进程调度的时间片，再运行同样的样本程序，体会不同时间片带来的差异

相关文件  
init/main.c kernel/fork.c kernel/sched.c

进程状态切换点
kernel/fork.c,sched.c,exit.c

#### 3.1 process.c文件的编写
先来看给出的帮助函数void cpuio_bound(int last,int cpu_time,int io_time)
```c
// 此程序模拟了占用CPU的实际和I/O时间
void cpuio_bound(int last, int cpu_time, int io_time)
{
	struct tms start_time, current_time;
	clock_t utime, stime;
	int sleep_time;

	while (last > 0)
	{
		/* CPU Burst */
		times(&start_time);
		/* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
		 * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
		 * 加上很合理。*/
		do
		{
			times(&current_time);
			utime = current_time.tms_utime - start_time.tms_utime;
			stime = current_time.tms_stime - start_time.tms_stime;
		} while ( ( (utime + stime) / HZ )  < cpu_time );
		last -= cpu_time;

		if (last <= 0 )
			break;

		/* IO Burst */
		/* 用sleep(1)模拟1秒钟的I/O操作 */
		sleep_time=0;
		while (sleep_time < io_time)
		{
			sleep(1);
			sleep_time++;
		}
		last -= sleep_time;
	}
}
```
在上述程序基础上编写Ubuntu下的测试程序



process.log出了点问题，0进程一直打印  
sys_pause函数中加上一条判断，不再输出进程0的循环等待信息

调度时间片修改在p->priority上

process.log为输出的日志文件

Linux 0.11上的gcc好像不支持双斜线注释


#### 3.2 修改Linux 0.11源码实现进程轨迹跟踪
按照顺序修改Linux 0.11内核代码
首先按照实验手册6.2指导，为在内核启动时就打开log文件开始记录，直接在进程0中就增加log文件的打开以及文件描述符的绑定
```c
//在 init/main.c中增加如下部分

move_to_user_mode();
/********修改部分********/
setup((void *) &drive_info);	//加载文件系统
(void) open("/dev/tty0",O_RDWR,0);	//打开tty0(见注释2)，将tty0和文件描述符0关联
(void) dup(0);	// 将文件描述符1和/dev/tty0关联（见注释3）
(void) dup(0);

(void) open("./var/process.log",O_CREAT|O_TRUNC|O_WRONLY,0666);
/********修改结束********/

// 这一部分是从init()中的对应部分移植过来的，因此删除init中的对应部分
```

在内核状态下，无法使用write()函数，正如printf()和printk()之间的区别。这里6.3中直接给出了fprintk()函数作为参考，这一函数参考了printk()和sys_write()。

Q:printf()和printk()的什么区别导致了使用场景不同？(见注释5)

### 注释
#### 1. tms结构体
```c
struct tms{
	clock_t tms_utime;//用户CPU时间    这里的clock_t类型是long int
 	clock_t tms_stime;//系统CPU时间
    clock_t tms_cutime;//终止的子进程，用户CPU时间
    clock_t tms_cstime;//终止的子进程，系统CPU时间
  };
```

#### 2. /dev/tty0
Linux系统中存放于/dev目录下的设备文件，代表连接到系统的硬件设备  
tty是TeleTYpewriter的缩写，起源于早期用于连接计算机的终端，/dev/tty是一个特殊的文件，代表当前进程的终端，它显示与当前会话关联的终端  
/dev/tty0是默认的虚拟控制台
参考 [知乎文章](https://zhuanlan.zhihu.com/p/632099551)

#### 3. dup函数
```c
#include <unistd.h>
int dup(int oldfd);
int dup2(int oldfd, int newfd);

返回值：
	成功：dup函数返回当前系统可用的最小整数值。
		 dup2函数返回第一个不小于newfd的整数值，分两种情况：
		 	1. 如果newfd已经打开，则先将其关闭，再复制文件描述符；
		 	2. 如果newfd等于oldfd，则dup2函数返回newfd，而不关闭它。
	失败：dup和dup2函数均返回-1，并设置errno。
```

#### 4. open函数及其参数
```c
int open(char* path,int access,int auth)
/*
	path: 文件路径
	access: 文件的打开方式
	auth: 可选参数，访问权限

	access的可选项包括
	O_RDONLY,O_WRONLY,O_RDWR
	O_CREAT,O_TRUNC（打开时将原有内容全部丢弃），O_APPEND等
*/
// 示例代码中的语句如下
(void) open("/var/process.log",O_CREAT|O_TRUNC|O_WRONLY,0666);
```


#### 5. printf()与printk()
printf()函数在main()中定义，主要功能是调用vsprintf()函数对字符串进行处理，然后调用系统调用write将其输出到文件描述符1（stdout）  
vsprintf()在./kernel/vsprintf.c中实现
```c
int vsprintf(char *str, const char *format, va_list arg)
/*
	str: 指向一个字符数组的指针，该数组存储了C字符串
	format: 对字符串进行格式化的参数
	arg: 表示可变参数列表

	返回值: 如果写入成功，返回写入的字符串总数，否则返回一个负数
*/
```

printk()函数在./kernel/printk.c中实现，主要是调用了tty_write函数
tty_write()函数在./kernel/chr_drv/tty_io.c中实现