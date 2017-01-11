#include "FreeRTOSMallocFixer.h"

#ifdef ARDUINO
#include <Arduino_FreeRTOS.h>
#else
#include <FreeRTOS.h>
#endif

#include <stdlib.h>

struct __freelist
{
  size_t             sz;
  struct __freelist *nx;
};

#ifdef __cplusplus
extern "C" {
#endif

extern size_t __malloc_margin;
extern char * __malloc_heap_start;
extern char * __malloc_heap_end;
extern char * __brkval;     /* first location not yet allocated */
extern struct __freelist *__flp; /* freelist pointer (head of freelist) */

// Use the normal free
#ifdef free
#undef free
#endif

#ifdef __cplusplus
}
#endif

static void *
_mallocFixer(size_t len)
{
  struct __freelist *fp1, *fp2, *sfp1, *sfp2;
  char * cp;
  size_t s, avail;

  /*
   * Our minimum chunk size is the size of a pointer (plus the
   * size of the "sz" field, but we don't need to account for
   * this), otherwise we could not possibly fit a freelist entry
   * into the chunk later.
   */
  if(len < sizeof(struct __freelist) - sizeof(size_t))
  {
    len = sizeof(struct __freelist) - sizeof(size_t);
  }

  /*
   * First, walk the free list and try finding a chunk that
   * would match exactly.  If we found one, we are done.  While
   * walking, note down the smallest chunk we found that would
   * still fit the request -- we need it for step 2.
   *
   */
  for(s = 0, fp1 = __flp, fp2 = 0;
      fp1;
      fp2 = fp1, fp1 = fp1->nx)
  {
    if(fp1->sz < len)
    {
      continue;
    }

    if(fp1->sz == len)
    {
      /*
       * Found it.  Disconnect the chunk from the
       * freelist, and return it.
       */
      if(fp2)
      {
        fp2->nx = fp1->nx;
      }
      else
      {
        __flp = fp1->nx;
      }

      return &(fp1->nx);
    }
    else
    {
      if(s == 0 || fp1->sz < s)
      {
        /* this is the smallest chunk found so far */
        s    = fp1->sz;
        sfp1 = fp1;
        sfp2 = fp2;
      }
    }
  }

  /*
   * Step 2: If we found a chunk on the freelist that would fit
   * (but was too large), look it up again and use it, since it
   * is our closest match now.  Since the freelist entry needs
   * to be split into two entries then, watch out that the
   * difference between the requested size and the size of the
   * chunk found is large enough for another freelist entry; if
   * not, just enlarge the request size to what we have found,
   * and use the entire chunk.
   */
  if(s)
  {
    if(s - len < sizeof(struct __freelist))
    {
      /* Disconnect it from freelist and return it. */
      if(sfp2)
      {
        sfp2->nx = sfp1->nx;
      }
      else
      {
        __flp = sfp1->nx;
      }

      return &(sfp1->nx);
    }

    /*
     * Split them up.  Note that we leave the first part
     * as the new (smaller) freelist entry, and return the
     * upper portion to the caller.  This saves us the
     * work to fix up the freelist chain; we just need to
     * fixup the size of the current entry, and note down
     * the size of the new chunk before returning it to
     * the caller.
     */
    cp       = (char *)sfp1;
    s       -= len;
    cp      += s;
    sfp2     = (struct __freelist *)cp;
    sfp2->sz = len;
    sfp1->sz = s - sizeof(size_t);

    return &(sfp2->nx);
  }

  /*
   * Step 3: If the request could not be satisfied from a
   * freelist entry, just prepare a new chunk.  This means we
   * need to obtain more memory first.  The largest address just
   * not allocated so far is remembered in the brkval variable.
   * Under Unix, the "break value" was the end of the data
   * segment as dynamically requested from the operating system.
   * Since we don't have an operating system, just make sure
   * that we don't collide with the stack.
   */
  if(__brkval == 0)
  {
    __brkval = __malloc_heap_start;
  }

  /*
   * Comment out all cp checking! That conflicted with FreeRTOS lead can't
   * allocate memory from tasks. (return 0)
   */

//  cp = __malloc_heap_end;

//  if(cp == 0)
//  {
//    cp = STACK_POINTER() - __malloc_margin;
//  }

//  if (cp <= __brkval)
//    /*
//     * Memory exhausted.
//     */
//    return 0;
//  avail = cp - __brkval;
//  /*
//   * Both tests below are needed to catch the case len >= 0xfffe.
//   */
//  if (avail >= len && avail >= len + sizeof(size_t)) {
  fp1       = (struct __freelist *)__brkval;
  __brkval += len + sizeof(size_t);
  fp1->sz   = len;
  return &(fp1->nx);
//  }
//  /*
//   * Step 4: There's no help, just fail. :-/
//   */
//  return 0;
}

void *
_freeRTOSMallocFixer(size_t len)
{
  void *ret = NULL;

  vTaskSuspendAll();
  {
    ret = _mallocFixer(len);
  }
  (void)xTaskResumeAll();

  return ret;
}

void
_freeRTOSFreeFixer(void *ptr)
{
  vTaskSuspendAll();
  free(ptr);
  xTaskResumeAll();
}
