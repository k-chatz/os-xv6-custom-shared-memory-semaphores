#define SHMKEYSIZE      50     // Maximum shared memory key size
#define SHMKEYMINSIZE   16     // Minimum sheared memory key size

struct shmkey {
    uint size;
    char v[SHMKEYSIZE];
};

