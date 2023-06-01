### Lab 2 系统调用
在Linux 0.11上添加两个系统调用
- iam()
    int iam(const char * name)  
    将name中的内容拷贝到操作系统中并保存下来
- whoami()
    int whoami(const char * name, unsigned int size)

系统调用的过程
- 将系统调用的编号存入eax中
- 把函数参数存入其他通用寄存器
- 触发0x80中断

```c
# close的调用过程
int close(int fd)
{
    long __res;
    __asm__ volatile ("int $0x80"
        : "=a" (__res)
        : "0" (__NR_close),"b" ((long)(fd)));
    if (__res >= 0)
        return (int) __res;
    errno = -__res;
    return -1;
}
```
它先将宏 __NR_close 存入 EAX，将参数 fd 存入 EBX，然后进行 0x80 中断调用。调用返回后，从 EAX 取出返回值，存入 __res，再通过对 __res 的判断决定传给 API 的调用者什么样的返回值。


注：《注释》3.6节及第四章

在用户态和内核态之间传递数据
```c
# open的调用过程
int open(const char * filename, int flag, ...)
{
//    ……
    __asm__("int $0x80"
            :"=a" (res)
            :"0" (__NR_open),"b" (filename),"c" (flag),
            "d" (va_arg(arg,int)));
//    ……
}
```
- eax寄存器传递系统调用号
- ebx对应第一个参数，ecx对应第二个参数，以此类推

get_fs_byte()和put_fs_byte()

用printk()调试内核

gcc -o iam iam.c -Wall

硬盘挂载
oslab/
sudo ./mount-hdc
sudo umount hdc

遇到的问题
- iam.c和whoami.c需要通过挂载硬盘的方式放到 ./hdc/usr/root 目录下并在Linux 0.11上编译
- iam.c和whoami.c中需要写main函数，和open的调用还是不一样的