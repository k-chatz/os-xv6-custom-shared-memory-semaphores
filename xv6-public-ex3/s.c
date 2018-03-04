#include "types.h"
#include "stat.h"
#include "user.h"
#include "sh_key_t.h"

#define SIZE 4096

int main(int argc, char *argv[]) {
    int i = 0, children = 2, pid = 0, wpid = 0, pool = 100, x = 0;
    char *mem = 0;
    sem_t *sem = 0;
    sh_key_t key;

    shm_key_init(&key, "AAAAAAAAAA\0", 16);
    mem = shmget(key);

    sem_init(sem, 0);

    memmove(mem, sem, sizeof(sem));

    printf(1, "sizeof(sem)=%d - sem=%d\n", sizeof(sem), sem);

    // Critical section start
    sem_down(sem);
    memmove(mem + sizeof(sem), &pool, sizeof(int));
    sem_up(sem);
    // Critical section end


    printf(1, "\npid: %d - Now i create %d children...\n", getpid(), children);
    sleep(500);

    for (i = 0; i < children; ++i) {
        pid = fork();
        if (pid == 0)
            break;
    }

    if (pid < 0) {
        printf(1, "\npid: %d - fork fail!\n", getpid());
        exit();
    } else if (pid > 0) {
        //printf(1, "\npid: %d - I'm the parent process...\n", getpid());
        while ((wpid = wait()) > 0 /*&& wpid != pid*/) {
            memmove(&pool, mem + sizeof(sem), sizeof(int));
            //printf(1, "\npid: %d - After execution of process:%d, the level is: %d\n", getpid(), wpid, pool);
        }

        memmove(&pool, mem + sizeof(sem), sizeof(int));
        printf(1, "\npid: %d - I finish my job, the level is: %d\n", getpid(), pool);


    } else if (pid == 0) {
        sleep(500);
        pool = 0;

        // Critical section start
        sem_down(sem);
        printf(1, "pid: %d - I'm the child process::\t", getpid());
        memmove(&pool, mem + sizeof(sem), sizeof(int));
        x = 26;
        if (x <= pool) {
            printf(1, "yes, I did it! new pool value is %d\n", pool);
            pool -= x;
            memmove(mem + sizeof(sem), &pool, sizeof(int));
        } else {
            printf(1, "The transaction could not be completed, ask for less than %d\n", pool);
        }
        sem_up(sem);
        // Critical section end
    }

    exit();
}
