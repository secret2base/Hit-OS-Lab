### Lab 1 控制系统启动
- 阅读《Linux内核完全注释》的第六章
- 按照要求改写0.11的引导程序bootsect.s
  要求屏幕上打印一段提示信息 "XXX is booting..."
- (改写进入保护模式前的设置程序setup.s)
  - bootsect.s能完成setup.s的载入，并跳转到setup.s开始地址执行。setup.s向屏幕输出一行"Now we are in SETUP"
  - setup.s能获取至少一个基本硬件参数，将其存放在内存的特定地址并输出到屏幕上
  - setup.s不再加载Linux内核，保持上述信息显示在屏幕上即可

回答问题：
有时，继承传统意味着别手蹩脚。x86 计算机为了向下兼容，导致启动过程比较复杂。请找出 x86 计算机启动过程中，被硬件强制，软件必须遵守的两个“多此一举”的步骤（多找几个也无妨），说说它们为什么多此一举，并设计更简洁的替代方案。

相关文件
boot/bootsect.s, boot/setup.s, tools/build.c


#### 1.as86和ld86
在Linux 0.x系统中使用了两种汇编器，一种是能产生16位代码的as86汇编器，使用配套的ld86连接器。另一种是GNU的汇编器gas  
Linux系统仅用as86和ld86创建16位的启动引导扇区  
汇编的基本格式为
```
as [选项] -o objfile srcfile
```
-0 以16位代码段运行
-a 使汇编程序部分兼容于Minix
-o 生成目标文件（目标文件名写在后面）

注：Ubuntu 18.04缺少相应环境 参考https://blog.csdn.net/qq_40758751/article/details/88707214 解决
