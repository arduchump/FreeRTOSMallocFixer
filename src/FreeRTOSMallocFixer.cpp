#include "FreeRTOSMallocFixer.h"

#ifdef ARDUINO
#include <Arduino_FreeRTOS.h>
#else
#include <FreeRTOS.h>
#endif

#include <stdlib.h>
#include <string.h>

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

static void *
_reallocFixer(void *ptr, size_t len)
{
  struct __freelist *fp1, *fp2, *fp3, *ofp3;
  char * cp, *cp1;
  void * memp;
  size_t s, incr;

  /* Trivial case, required by C standard. */
  if(ptr == 0)
  {
    return _mallocFixer(len);
  }

  cp1  = (char *)ptr;
  cp1 -= sizeof(size_t);
  fp1  = (struct __freelist *)cp1;

  cp = (char *)ptr + len;   /* new next pointer */

  if(cp < cp1)
  {
    /* Pointer wrapped across top of RAM, fail. */
    return 0;
  }

  /*
   * See whether we are growing or shrinking.  When shrinking,
   * we split off a chunk for the released portion, and call
   * free() on it.  Therefore, we can only shrink if the new
   * size is at least sizeof(struct __freelist) smaller than the
   * previous size.
   */
  if(len <= fp1->sz)
  {
    /* The first test catches a possible unsigned int
     * rollover condition. */
    if(fp1->sz <= sizeof(struct __freelist) ||
       len > fp1->sz - sizeof(struct __freelist))
    {
      return ptr;
    }

    fp2     = (struct __freelist *)cp;
    fp2->sz = fp1->sz - len - sizeof(size_t);
    fp1->sz = len;
    free(&(fp2->nx));
    return ptr;
  }

  /*
   * If we get here, we are growing.  First, see whether there
   * is space in the free list on top of our current chunk.
   */
  incr = len - fp1->sz;
  cp   = (char *)ptr + fp1->sz;
  fp2  = (struct __freelist *)cp;

  for(s = 0, ofp3 = 0, fp3 = __flp;
      fp3;
      ofp3 = fp3, fp3 = fp3->nx)
  {
    if(fp3 == fp2 && fp3->sz + sizeof(size_t) >= incr)
    {
      /* found something that fits */
      if(fp3->sz + sizeof(size_t) - incr > sizeof(struct __freelist))
      {
        /* split off a new freelist entry */
        cp      = (char *)ptr + len;
        fp2     = (struct __freelist *)cp;
        fp2->nx = fp3->nx;
        fp2->sz = fp3->sz - incr;
        fp1->sz = len;
      }
      else
      {
        /* it just fits, so use it entirely */
        fp1->sz += fp3->sz + sizeof(size_t);
        fp2      = fp3->nx;
      }

      if(ofp3)
      {
        ofp3->nx = fp2;
      }
      else
      {
        __flp = fp2;
      }

      return ptr;
    }

    /*
     * Find the largest chunk on the freelist while
     * walking it.
     */
    if(fp3->sz > s)
    {
      s = fp3->sz;
    }
  }

  /*
   * If we are the topmost chunk in memory, and there was no
   * large enough chunk on the freelist that could be re-used
   * (by a call to malloc() below), quickly extend the
   * allocation area if possible, without need to copy the old
   * data.
   */
  if(__brkval == (char *)ptr + fp1->sz && len > s)
  {
    cp1 = __malloc_heap_end;
    cp  = (char *)ptr + len;

//    if(cp1 == 0)
//    {
//      cp1 = STACK_POINTER() - __malloc_margin;
//    }

//    if(cp < cp1)
//    {
    __brkval = cp;
    fp1->sz  = len;
    return ptr;
//  }

    /* If that failed, we are out of luck. */
//    return 0;
  }

/*
 * Call malloc() for a new chunk, then copy over the data, and
 * release the old region.
 */
  if((memp = _mallocFixer(len)) == 0)
  {
    return 0;
  }

  memcpy(memp, ptr, fp1->sz);

  free(ptr);
  return memp;
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

void *
_freeRTOSCallocFixer(size_t nmemb, size_t size)
{
  void *ptr = NULL;

  vTaskSuspendAll();

  ptr = _mallocFixer(nmemb * size);

  if(ptr)
  {
    memset(ptr, 0, nmemb * size);
  }

  xTaskResumeAll();

  return ptr;
}

void *
_freeRTOSReallocFixer(void *ptr, size_t len)
{
  vTaskSuspendAll();
  ptr = _reallocFixer(ptr, len);
  xTaskResumeAll();

  return ptr;
}
