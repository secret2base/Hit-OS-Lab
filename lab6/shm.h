#include <stddef.h>     
 
typedef unsigned int key_t;
 
struct struct_shmem
{
    unsigned int size;
    unsigned int key;
    unsigned long page;
};
 
int shmget(key_t key, size_t size);
void* shmat(int shmid);
 
#define SHM_NUM  16 