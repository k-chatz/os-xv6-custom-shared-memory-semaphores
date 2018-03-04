#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "shmem.h"

struct {
    struct spinlock lock;
    struct shmpg pages[NSHMPG];
} shmtable;


void shmkeyinit(sh_key_t k1) {
    int i;
    k1->size = 0;
    for (i = 0; i < SHMKEYSIZE; ++i)
        k1->v[i] = 0;
}

void shmkeycpy(sh_key_t k1, sh_key_t k2) {
    int i;
    k1->size = k2->size;
    for (i = 0; i < k2->size; ++i)
        k1->v[i] = k2->v[i];
}

int shmkeycmp(sh_key_t k1, sh_key_t k2) {
    int i;
    if (k1->size == k2->size) {
        for (i = 0; i < k2->size; ++i)
            if (k1->v[i] != k2->v[i])
                return 0;
        return 1;
    }
    return 0;
}

//shminit
void shminit(void) {
    struct shmpg *page;
    acquire(&shmtable.lock);
    for (page = shmtable.pages; page < &shmtable.pages[NSHMPG]; page++) {
        page->pa = 0;
        page->usages = 0;
        page->allocated = 0;
    }
    release(&shmtable.lock);
    cprintf("initialization of shared memory has completed\n");
}

struct shmpg *pgalloc(sh_key_t key) {
#if (SHMD)
    cprintf("shm::pgalloc::  \t");
#endif
    struct shmpg *page;
    acquire(&shmtable.lock);
    for (page = shmtable.pages; page < &shmtable.pages[NSHMPG]; page++) {
        if (page->pa == 0 || shmkeycmp(&page->key, key)) {
            shmkeycpy(&page->key, key);
            page->pa = kalloc();
            page->allocated = 1;
            if (page->pa == 0)
                panic("pgalloc: kalloc");
            memset(page->pa, 0, PGSIZE);
            release(&shmtable.lock);
#if (SHMD)
            pgprint(page);
            cprintf("\n");
#endif
            return page;
        }
    }
    release(&shmtable.lock);
#if (SHMD)
    cprintf("FAIL!\n");
#endif
    panic("shmget: shm table full");
}

int pgfind(sh_key_t key) {
#if (SHMD)
    cprintf("shm::pgfind::   ");
#endif
    struct shmpg *page;
    acquire(&shmtable.lock);
    int j = 0;
    for (page = shmtable.pages; page < &shmtable.pages[NSHMPG]; page++) {
        if (shmkeycmp(&page->key, key)) {
            release(&shmtable.lock);

            if (!page->allocated) {
                page = pgalloc(key);
            }
#if (SHMD)
            cprintf("\tPage found in position: %d\n", j);
#endif
            return j;
        }
        j++;
    }
    release(&shmtable.lock);
#if (SHMD)
    cprintf("\tPage not found!\n");
#endif
    return -1;
}

void pgprint(struct shmpg *page) {
    if (page != 0) {
        cprintf(
                "{'page': {'pa':%d, 'usages':%d, 'key': {'size':%d, 'v':'%s'}}}",
                (int) page->pa, (int) page->usages, (int) page->key.size, page->key.v
        );
    } else
        cprintf("shm::pgprint::\tpage is null\n");
}


void pgdealloc(struct shmpg *page) {
#if (SHMD)
    cprintf("shm::pgdealloc::\tBefore:");
    pgprint(page);
    cprintf(" After:");
#endif
    if (page->pa) {
        shmkeyinit(&page->key);
        kfree(page->pa);
        page->pa = 0;
    }
#if (SHMD)
    pgprint(page);
    cprintf("\n\n");
#endif
}

void shm_key_init(sh_key_t *key, char *v, uint size) {
#if (SHMD)
    cprintf("shm::shm_key_init::\t\n");
#endif
    struct shmpg *page;
    int j = 0;
    *key = 0;
    acquire(&shmtable.lock);
    for (page = shmtable.pages; page < &shmtable.pages[NSHMPG]; page++) {
        if (page->pa == 0) {
            for (j = 0; j < size; ++j) {
                page->key.v[j] = v[j];
            }
            page->key.size = j;
            *key = &page->key;
            break;
        }
    }
    release(&shmtable.lock);
}

void *shmget(sh_key_t key) {
#if (SHMD)
    cprintf("\nshm::shmget::   \n");
#endif
    char *va = 0;
    struct shmpg *page = 0;
    int pgindex = pgfind(key);
    if (pgindex >= 0) {
        if (shmtable.pages[pgindex].usages <= NSHMPGPROC)
            page = &shmtable.pages[pgindex];
        else
            return (void *) va;
    } else
        page = pgalloc(key);
    va = shmpgmap(0, -1, page);
#if (SHMD)
    cprintf("shm::shmget::   \tREFERENCE PAGE:\t");
    pgprint(page);
    cprintf("\n");
#endif
    return (void *) va;
}

int shmrem(sh_key_t key) {
#if (SHMD)
    cprintf("\nshm::shmrem::   \n");
#endif
    struct shmpg *page = 0;
    int usages;
    int pgindex = pgfind(key);
    if ((pgindex >= 0) && (shmtable.pages[pgindex].usages > 0)) {
        page = &shmtable.pages[pgindex];
        usages = shmpgunmap(page, -1);
        if (usages == 0)
            pgdealloc(page);
#if (SHMD)
        cprintf("shm::shmrem::   \tRemain: %d\n\n", usages);
#endif
        return usages;
    } else
        panic("page remove");
}


