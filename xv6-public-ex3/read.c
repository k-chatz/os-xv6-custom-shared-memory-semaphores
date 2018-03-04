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


int main(void) {

    sem_t sem;

    sem_init(&sem, 0);

    char *mem;

    int j = 5;
    //for (int j = 1; j <= 32; ++j) {

    mem = shmget(j);

    printf(1, "PID: %d\tshmget(%d) - mem = %d\n", getpid(), j, mem);

    sem_up(&sem);

    struct point points[POINTS];
    memmove(&points, mem, sizeof(points));
    print_points(points);

    sem_down(&sem);



    //}

    exit();
}
