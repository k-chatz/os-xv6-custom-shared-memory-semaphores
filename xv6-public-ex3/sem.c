#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "sem.h"

void sem_init(sem_t *sem, int value) {
    static struct semtype s;
    s.count = value;
    //cprintf("sem::sem_init::\ts.count=%d\n", s.count);
    initlock(&s.lk, "sem lock");
    s.lk.locked = 0;
    *sem = s;
}

void sem_down(sem_t *sem) {
    acquire(&sem->lk);
    sem->count--;
    while (sem->count < 0) {
        cprintf("+\0");
        sleep(sem, &sem->lk);
    }
    //cprintf("sem::sem_down::\tSTART:sem->count=%d\n", sem->count);
    release(&sem->lk);
}


void sem_up(sem_t *sem) {
    acquire(&sem->lk);
    sem->count++;
    wakeup(sem);
    release(&sem->lk);
    //cprintf("sem::sem_up::\tSTART:sem->count=%d\n", sem->count);
}