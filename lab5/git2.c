#define __LIBRARY__

#include <stdio.h>
#include <sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t*, sem);
_syscall1(int, sem_post, sem_t*, sem);
_syscall1(int, sem_unlink, const char*, name);

#define BUFSIZE 10  

#define CONSUMER_NUM 4  

#define MAX_NUM 500

int fd;
sem_t *empty; 
sem_t *full; 
sem_t *mutex;  

int ftruncate(int fd, unsigned long size)
{
    return fcntl(fd, F_CHGSIZE, size);
}

void producer()
{
    int item_num = 0;
    while (item_num < MAX_NUM)
    {
        sem_wait(empty);
        sem_wait(mutex);
        if (lseek(fd, 0, SEEK_END) < 0)
            fprintf(stderr, "Error in producer lseek\n");
        write(fd, &item_num, sizeof(int));
        sync();
        sem_post(mutex);
        sem_post(full);
        ++item_num;
    }
    close(fd);
}

void consumer()
{
    int item;
    int file_len;
    int tmp_value;
    int j;

    do
    {
        sem_wait(full);
        sem_wait(mutex);
        if (lseek(fd, 0, SEEK_SET) < 0)
            fprintf(stderr, "Error in consumer lseek\n");

        if (read(fd, &item, sizeof(int)) == 0)
        {
            sem_post(mutex);
            sem_post(empty);
            break;
        }

        printf("%d: %d\n", getpid(), item);
        fflush(stdout);

        file_len = lseek(fd, 0, SEEK_END); 
        if (file_len < 0)
            fprintf(stderr, "Error when get file length\n");
        for(j = 1; j < (file_len / sizeof(int)); j++) 
        { 
            lseek(fd, j * sizeof(int), SEEK_SET); 
            read(fd, &tmp_value, sizeof(int)); 
            lseek(fd, (j - 1) * sizeof(int), SEEK_SET); 
            write(fd, &tmp_value, sizeof(int)); 
        } 
        ftruncate(fd, file_len - sizeof(int)); 

        sem_post(mutex);
        sem_post(empty);
    }while(item < MAX_NUM - 1);
    sem_post(full);
    close(fd);
}

int main()
{
    char empty_name[20];
    char full_name[20];
    char mutex_name[20];
    int i;
    pid_t p_pid;  

    /* from APUE */
    sprintf(empty_name, "/%ld_empty", (long)getpid());    
    sprintf(full_name, "/%ld_full", (long)getpid());
    sprintf(mutex_name, "/%ld_mutex", (long)getpid());

    fd = open("share.file", O_CREAT | O_RDWR | O_TRUNC, 0666);

    empty = sem_open(empty_name, BUFSIZE);
    if (empty == NULL)
    {
        fprintf(stderr, "Error when create empty\n");
        exit(0);
    }
    full = sem_open(full_name, 0);
    if (full == NULL)
    {
        fprintf(stderr, "Error when create full\n");
        exit(0);
    }
    mutex = sem_open(mutex_name, 1);
    if (mutex == NULL)
    {
        fprintf(stderr, "Error when create mutex\n");
        exit(0);
    }

    printf("Create semphore OK!\n");

    for (i = 0; i < CONSUMER_NUM; ++i)
    {
        if (!fork())
        {
            consumer();
            exit(0);
        }
    }

    if (!fork())
    {
        producer();
        exit(0);
    }
    while (waitpid(-1, NULL, 0) > 0)
        ;

    sem_unlink(empty_name);
    sem_unlink(full_name);
    sem_unlink(mutex_name);
    close(fd);
    
    return 0;
}