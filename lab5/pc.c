#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<semaphore.h>
#include<fcntl.h>

int main(int argc, char** argv){
    const char* path="buffer.txt";
    int value;
    int fd;
    fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0666);

    sem_unlink("/myEmpty");
    sem_unlink("/myFull");
    sem_unlink("/myMutex");
    sem_t* empty = sem_open("/myEmpty", O_CREAT, 0666, 10);
    sem_getvalue(empty,&value);
    printf("empty: %d.\n",value);
    sem_t* full = sem_open("/myFull",O_CREAT, 0666,0);
    sem_t* mutex = sem_open("/myMutex",O_CREAT, 0666,1);  //1是空闲，0是正在占用中
    sem_getvalue(mutex,&value);
    printf("mutex: %d.\n",value);

    pid_t n_proc[5];
    pid_t c;
    c=fork();
    if(c==0){
        int i;
        for(i=0;i<50;i++){
            sem_wait(empty);
            sem_wait(mutex);

            printf("Processor is writing %d.\n",i);
            lseek(fd,0,SEEK_END);
            write(fd,&i,sizeof(int));

            sem_post(mutex);
            sem_post(full);
        }
        return;
    }
    if(c<0) printf("Consumer Create Failure.\n");
    int i;
    for(i=0;i<5;i++){
        n_proc[i]=fork();
        if(n_proc[i]==0){
            int j,tmp;
            for(j=0;j<10;j++){
                sem_wait(full);
                sem_wait(mutex);

                lseek(fd,0,SEEK_SET);
                read(fd,&tmp,sizeof(int));
                printf("Consumer %d reading : %d.\n",getpid(),tmp);
                int file_len=lseek(fd,0,SEEK_END);
                int k;
                for(k=1;k<(file_len/sizeof(int));k++){
                    lseek(fd,k*sizeof(int),SEEK_SET);
                    read(fd,&tmp,sizeof(int));
                    lseek(fd,(k-1)*sizeof(int),SEEK_SET);
                    write(fd,&tmp,sizeof(int));
                }
                ftruncate(fd, file_len - sizeof(int));

                sem_post(mutex);
                sem_post(empty);
            }
            return;
        }
        if(n_proc[i]<0) printf("Process Creat Failure.\n");
    }

    /* wait(NULL); */
    while (waitpid(-1, NULL, 0) > 0);
    sem_unlink("/myEmpty");
    sem_unlink("/myMutex");
    sem_unlink("/myFull");
    close(fd);
    return 0;
}
