#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/types.h>
int main() {
    struct tms start_tms;
    struct tms end_tms;
//once we have established the pipeline we fork the child
    pid_t   childpid;
times(&start_tms);
printf("Test start_tms.tms_utime = %jd\n\n",  (intmax_t)start_tms.tms_utime);
    printf("Test start_tms.tms_cutime = %jd\n\n", (intmax_t)start_tms.tms_cutime);
    printf("Test start_tms.tms_stime = %jd\n\n",  (intmax_t)start_tms.tms_stime);
    printf("Test start_tms.tms_cstime = %jd\n\n",  (intmax_t)start_tms.tms_cstime);
if((childpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }
if(childpid == 0)
    {
        //execute /bin/ls
        execl("/bin/ls", "/bin/ls", "-R", "-t", "-l", (char *) 0);
        exit(0);
    }
    else
    {
        /* Parent process */
/* NEW MODIFICATION, wait for the child!!! */
        if (waitpid(childpid,NULL,0) == -1)
        {
            perror("waitpid");
            exit(1);
        }
/* call times for capturing end times */
        times(&end_tms);
printf("Test end_tms.tms_utime = %jd\n\n",end_tms.tms_utime);
        printf("Test end_tms.tms_cutime = %jd\n\n",end_tms.tms_cutime);
        printf("Test end_tms.tms_stime = %jd\n\n",end_tms.tms_stime);
        printf("Test end_tms.tms_cstime = %jd\n\n",end_tms.tms_cstime);
/* Calculate times, unfortunately return zero, but why??? */
        clock_t cpu_time = end_tms.tms_cutime - start_tms.tms_cutime;
        clock_t utime = end_tms.tms_utime - start_tms.tms_utime;
        clock_t stime = end_tms.tms_stime - start_tms.tms_stime;
        clock_t cstime = end_tms.tms_cstime - start_tms.tms_cstime;
//Unfortunately printfs return zero, but why???
        printf("cpu time %jd\n\n",  (intmax_t)cpu_time);
        printf("cpu Utime %jd\n\n", (intmax_t)utime);
        printf("cpu Stime %jd\n\n", (intmax_t)stime);
        printf("cpu CStime %jd\n\n", (intmax_t)cstime);
    }
}