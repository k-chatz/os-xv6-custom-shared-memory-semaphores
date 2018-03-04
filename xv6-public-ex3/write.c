#include "types.h"
#include "stat.h"
#include "user.h"
#include "sh_key_t.h"
#include "sem.h"

#define SIZE 4096

#define POINTS 50

void print(char *mem) {
    int i;
    for (i = 0; i < SIZE; ++i)
        printf(1, "%c", mem[i]);
    printf(1, "\n");
}

struct point {
    int x;
    int y;
    int z;
};

void print_points(struct point *points) {
    for (int i = 0; i < POINTS; ++i) {
        if (points[i].x && points[i].y && points[i].z)
            printf(1,
                   "points[%d]:\t{'point':{'x':%d, 'y':'%d', 'z':'%d'}}\n",
                   i, points[i].x, points[i].y, points[i].z);
    }
}

int main(int argc, char *argv[]) {




    char *mem;
    int rem, pid;
    struct point points[POINTS];

    for (int j = 1; j <= 32; ++j) {
        mem = shmget(j);
        printf(1, "shmget(%d)\tmem = %d\n", j, mem);
    }

    sleep(50);

    // give me memory
    mem = shmget(5);
    printf(1, "PID: %d\tIM PARENT! - mem = %d\n", getpid(), mem);
    memset(mem, 0, SIZE);

    points[0].x = 10;
    points[0].y = 15;
    points[0].z = 18;

    memmove(mem, &points, sizeof(points));
    //print_points(points);

    //sleep(100);

    printf(1,
           "_________________________________________FORK_______________________________________________________\n\n");

    pid = fork();

    if (pid < 0) {
        printf(1, "PID: %d\tFORK FAIL!\n", getpid());
        exit();
    } else if (pid > 0) {
        // remove memory from me!
        rem = shmrem(5);
        printf(1, "PID: %d\tRemain: %d\n", getpid(), rem);
        if (!rem)
            printf(1, "PID: %d\tI'm the last one!\n", getpid());

    } else if (pid == 0) {
        sleep(1000);
        printf(1, "PID: %d\tIM CHILD! - mem = %d\n", getpid(), mem);

        struct point points[POINTS];
        memmove(&points, mem, sizeof(points));

        points[1].x = 20;
        points[1].y = 25;
        points[1].z = 28;

        points[2].x = 30;
        points[2].y = 35;
        points[2].z = 38;

        memmove(mem, &points, sizeof(points));

        //print_points(points);
        printf(1,
               "I just put new data, now you can read them from other processes as long as I sleep until I finish!\n");
        sleep(2000);
    }
    printf(1,
           "____________________________________________________________________________________________________\n\n\n");
    exit();
}
