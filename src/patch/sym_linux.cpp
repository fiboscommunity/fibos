#ifdef linux

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <elf.h>
#include <errno.h>
#include <sys/syscall.h>
#include <linux/random.h>

extern "C" {

void __explicit_bzero_chk(void* dst, size_t len, size_t dstlen)
{
    memset(dst, '\0', len);
    /* Compiler barrier.  */
    asm volatile("" ::
                     : "memory");
}

unsigned long getauxval(unsigned long type)
{
    return 0;
}

void __chk_fail(void);
long int __fdelt_chk(long int d)
{
    if (d >= FD_SETSIZE || d < 0)
        __chk_fail();

    return d / __NFDBITS;
}

int pthread_setname_np(pthread_t thread, const char* name)
{
    return 0;
}

int pthread_getname_np(pthread_t thread, char* name, size_t len)
{
    static int thread_count = 0;

    sprintf(name, "thread-%d", thread_count++);
    return 0;
}

int pthread_mutexattr_setrobust(pthread_mutexattr_t* attr, int robust)
{
    return 0;
}

void __longjmp_chk()
{
}

void* aligned_alloc(size_t align, size_t size)
{
    if ((align & (align - 1)) || (size & (align - 1))) {
        errno = EINVAL;
        return NULL;
    }

    if (align < sizeof(void*))
        align = sizeof(void*);

    void* ptr;
    int err = posix_memalign(&ptr, align, size);
    if (err) {
        errno = err;
        ptr = NULL;
    }
    return ptr;
}
}
#endif