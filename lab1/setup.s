INITSEG  = 0x9000	! we move boot here - out of the way
SYSSEG   = 0x1000	! system loaded at 0x10000 (65536).
SETUPSEG = 0x9020	! this is the current segment

.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

entry start
start:
    mov	ax,#SETUPSEG	! this is done in bootsect already, but...
	mov	es,ax

    mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#26
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg2
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

!读光标位置 存入 0x9000:0

    mov ax,#INITSEG
    mov ds,ax
	mov	ah,#0x03	! read cursor pos
	xor	bh,bh
	int	0x10		! save it in known place, con_init fetches
	mov	[0],dx		! it from 0x90000.

!读内存大小 存入 0x9000:2
	mov	ah,#0x88
	int	0x15
	mov	[2],ax

!读显示器模式 存入 0x9000:4-6

	mov	ah,#0x0f
	int	0x10
	mov	[4],bx		! bh = display page
	mov	[6],ax		! al = video mode, ah = window width


	mov	ah,#0x12
	mov	bl,#0x10
	int	0x10
	mov	[8],ax
	mov	[10],bx
	mov	[12],cx

!读硬盘参数 第一个硬盘参数表的首地址是中断向量0x41的向量值

	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x41]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0080
	mov	cx,#0x10
	rep
	movsb

! Get hd1 data

	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x46]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0090
	mov	cx,#0x10
	rep
	movsb

!这里修改ds寄存器和es寄存器的值
    mov ax,#INITSEG
    mov ds,ax
    mov ax,#SETUPSEG
    mov es,ax

!字符串显示部分
    mov ax,[0]
    xor bh,bh
    int 0x10

    mov cx,#15
    mov bx,#0x0007
    mov bp,#cur
    mov ax,#0x1301
    int 0x10

!调用参考函数print_hex
    mov ax,[0]
    call print_hex
    call print_nl

!准备打印内存信息
    mov ah,#0x03
    xor bh,bh
    int 0x10

    mov cx,#12
    mov bx,#0x0007
    mov bp,#mem
    mov ax,#0x1301
    int 0x10

    mov ax,[2]
    call print_hex
    call print_nl

!准备打印硬盘信息
    mov ah,#0x03
    xor bh,bh
    int 0x10

    mov cx,#25
    mov bx,#0x0007
    mov bp,#cyl
    mov ax,#0x1301
    int 0x10

    mov ax,[0x80]
    call print_hex
    call print_nl

! Headers
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	mov	cx,#8
	mov	bx,#0x0007		! page 0, attribute c 
	mov	bp,#head
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

	mov ax,[0x80+0x02]
	call print_hex
	call print_nl

! Sectors
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	mov	cx,#8
	mov	bx,#0x0007		! page 0, attribute c 
	mov	bp,#sect
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

	mov ax,[0x80+0x0e]
	call print_hex
	call print_nl

!死循环
l:  jmp l

print_hex:
    mov cx,#4 !4个16进制数
    mov dx,ax !ax作为参数传递寄存器

print_digit:
    rol dx,#4  ! 循环以使低4比特用上 !! 取dx的高4比特移到低4比特处。
	mov ax,#0xe0f  ! ah = 请求的功能值,al = 半字节(4个比特)掩码。
	and al,dl ! 取dl的低4比特值。
	add al,#0x30  ! 给al数字加上十六进制0x30
	cmp al,#0x3a
	jl outp  !是一个不大于十的数字
	add al,#0x07  !是a~f,要多加7

outp:
	int 0x10
	loop print_digit
	ret
!打印换行和回车
print_nl:
	mov ax,#0xe0d
	int 0x10
	mov al,#0xa
	int 0x10
	ret

msg2:
	.byte 13,10
	.ascii "Now we are in SETUP."
	.byte 13,10,13,10


cur:
    .ascii "Cursor Postion:"

mem:
    .ascii "Memory Size:"

cyl:
	.ascii "KB"
	.byte 13,10,13,10
	.ascii "HD Info"
	.byte 13,10
	.ascii "Cylinders:"

head:
	.ascii "Headers:"
sect:
	.ascii "Secotrs:"


.text
endtext:
.data
enddata:
.bss
endbss:
