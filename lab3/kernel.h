/*
 * 'kernel.h' contains some often-used function prototypes etc
 */

// ./include/linux/kernel.c
void verify_area(void * addr,int count);
volatile void panic(const char * str);
int printf(const char * fmt, ...);
int printk(const char * fmt, ...);
int tty_write(unsigned ch,char * buf,int count);
void * malloc(unsigned int size);
void free_s(void * obj, int size);
int fprintk(int fd, const char *fmt, ...);

#define free(x) free_s((x), 0)

/*
 * This is defined as a macro, but at some point this might become a
 * real subroutine that sets a flag if it returns true (to do
 * BSD-style accounting where the process is flagged if it uses root
 * privs).  The implication of this is that you should do normal
 * permissions checks first, and check suser() last.
 */
#define suser() (current->euid == 0)

