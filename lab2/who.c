//位于kernel下
#include <string.h>
#include <errno.h>
#include <asm/segment.h>

char msg[24];

int sys_iam(const char* name){
    int i;
    char tmp[25];
    for(int i=0;i<25;i++){
        tmp[i]=get_fs_byte(name+i);
        if(tmp[i]=='\0') break;
    }

    i=0;
    while(i<25&&tmp[i]!='\0')   i++;
    if(i>23)    return -(EINVAL)
    strcpy(msg,tmp);
    return i;
}

int sys_whoami(char* name, unsigned int size){
    int len=0;
    while(len<24&&msg[len]!='\0')   len++;
    if(len>size)    return -(EINVAL)
    for(int i=0;i<size;i++){
        put_fs_byte(msg[i],name+i);
        if(msg[i]=='\0')    break;
    }
    return i;
}