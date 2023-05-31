! 按照实验要求重新编写bootsect.s
! 原bootsect.s过于全面,包含了太多冗余信息

! 定义全局标识符
.globl begtext, begdata, begbss, endtext, enddata, endbss
! begtext endtext 文本段
! begdata enddata 数据段
! begbss endbss 未初始化数据段(Block Started by Symbol)

.text
begtext:
.data
begdata:
.bss
begbss:
.text

! 定义关键常量
SETUPLEN = 4 ! Setup程序扇区的数值
BOOTSEG = 0x07c0 ! bootsect的原始地址，从硬盘读取后bottsect程序最开始位于此处
INITSEG = 0x9000 ! 随后bootsect被移动到此处
SETUPSEG = 0x9020 ! setup程序位于此处

! 变量定义结束，下面开始写bootsect的主要功能

entry start
start:
    mov ax,#BOOTSEG
    mov ds,ax   ! 上面两行将数据段寄存器ds的值设为 0x7c00
    mov ax,#INITSEG
    mov es,ax   ! 再将附加段寄存器es的值设为0x9000
    mov cx,#256 ! 移动数字，准备将bootsect搬到INITSEG处
    sub si,si   ! 源地址 ds:si = 0x7c00:0x0000
    sub di,di   ! 目的地址 es:di = 0x9000:0x0000
    rep
    movw    ! 重复执行移动一个字的操作，知道cx为0
    jmpi go,INITSEG ! 段间跳转，这里INITSEG给出了目的段的地址

! 从这里开始CPU执行位于0x9000处的代码了
go:
    mov ax,cs ! 代码段寄存器cs
    mov ds,ax
    mov es,ax
    mov ss,ax ! 数据段寄存器ds,附加段寄存器es,堆栈段寄存器ss

! 加载setup.s程序
! 将磁盘的第二个扇区读到0x90200处，共读入四个扇区
load_setup:
    mov dx,#0x0000 ! 驱动器 0 磁头 0
    mov cx,#0x0002 ! 扇道 2 磁道 0
    mov bx,#0x0200 ! INITSEG段 512偏移处
    mov ax,#0x0200 + SETUPLEN ! SETUPLEN = 4 四个扇区
    int 0x13 ! 0x13中断 将指定扇区加载到指定内存
    jnc ok_load_setup ! 若正常则继续
    mov dx,#0x0000
    mov ax,#0x0000 ! 复位磁盘
    int 0x13
    j load_setup

ok_load_setup:

! 读取功能写完了，下面写屏幕显示信息
    mov ah,#0x03 !读光标位置
    xor bh,bh 
    int 0x10

    mov cx,#25  ! cx寄存器放入字符串长度
    mov bx,#0x0007 
    mov bp,#msg1
    mov ax,#0x1301 ! 写字符串，移动光标
    int 0x10

! 完成后开始执行setup
    jmpi 0,SETUPSEG

msg1:
	.byte 13,10
	.ascii "LiOS is booting ..."
	.byte 13,10,13,10

.org 510
boot_flag:
	.word 0xAA55
.text
endtext:
.data
enddata:
.bss
endbss:

