#include "spinlock.h"

//semaphore type
struct semtype {
    uint count;
    struct spinlock lk;
};
