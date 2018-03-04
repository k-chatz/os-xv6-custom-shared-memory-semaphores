#include "sh_key_t.h"

struct shmpg {
    struct shmkey key;
    uint allocated;
    char *pa;       // physical address
    int usages;
};
