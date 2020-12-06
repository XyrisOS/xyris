#include <mem/paging.hpp>
#include <lib/mutex.hpp>
#include <lib/errno.h>
#include <stddef.h>

static px_mutex_t lock = { 0 };

#ifdef __cplusplus
extern "C" {
#endif

/*
 * lock the allocator.
 */
int liballoc_lock()
{
    int old_errno = errno;
    int result = px_mutex_lock(&lock);
    errno = old_errno;
    return result;
}

/*
 * unlock the allocator.
 */
int liballoc_unlock()
{
    int old_errno = errno;
    int result = px_mutex_unlock(&lock);
    errno = old_errno;
    return result;
}

/*
 * get a new page or pages.
 */
void *liballoc_alloc(int count)
{
    return px_get_new_page(count * PAGE_SIZE - 1);
}

/*
 * free a page or pages.
 */
int liballoc_free(void *page, int count)
{
    px_free_page(page, count * PAGE_SIZE - 1);
    return 0;
}

#ifdef __cplusplus
}
#endif

