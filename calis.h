/***********************************************************************
C implementation of Cache-Aware Local Iterative Sort (libcalis)
Author: Anseok Joo (anseok at gmail dot com)
Public Domain

Before including this header file, either typedef or #define calis_num
as the type of your array elements to be sorted.

On Unix and Linux, compile with -lm flag, so that math functions work.
***********************************************************************/

#ifndef _CALIS_H
#define _CALIS_H

#include <math.h>
#include <stdlib.h>
#include <string.h>

/** signatures of functions exported **********************************/
void calis_sort(
  calis_num src[],
  size_t size,
  calis_num dst[],
  calis_num *(*f)(calis_num [], size_t, size_t, calis_num [],
                  calis_num (*)(calis_num, calis_num), size_t, int),
  size_t cacheSize,
  calis_num (*cmp)(calis_num, calis_num));
  /* Sort list SRC of size SIZE and write to DST using sorting algorithm
     F using comparison function CMP assuming CPU cache size CACHESIZE.
     When DST == SRC, the sort is destructive. */

calis_num calis_cmp(
  calis_num a,
  calis_num b);
  /* Return -ve number if a < b; +ve numer if b > a; zero otherwise. */

calis_num *calis_mergesort(
  calis_num src[],
  size_t offset,
  size_t size,
  calis_num dst[],
  calis_num (*cmp)(calis_num, calis_num),
  size_t aSize,
  int iterations);
/* Sort using the merge sort algorithm using comparison function CMP.
   The list to be sorted is a subarray of SRC starting at OFFSET from
   SRC and is of length SIZE.  ASIZE is the smallest block size to
   consider.  The sort performs ITERATIONS levels of merge operations,
   full when ITERATIONS == 0.  The returned value is a pointer to the
   sorted list, which is either SRC or DST depending an the parity of
   the number of iterations. */

/** private constant **************************************************/
const size_t _calis_size = sizeof (calis_num);

/** function definitions **********************************************/
calis_num calis_cmp(calis_num a, calis_num b) { return a - b; }

size_t _calis_min(size_t a, size_t b) { return b < a ? b : a; }

void calis_sort(
  calis_num src[], size_t size, calis_num dst[],
  calis_num *(*f)(calis_num *, size_t, size_t, calis_num *,
                  calis_num (*)(calis_num, calis_num), size_t, int),
  size_t cacheSize, calis_num (*cmp)(calis_num, calis_num))
{ calis_num *ret, *tmp; size_t last, i, tmpSize;
  cacheSize /= 2;  /* since we work with two lists of same size */
  src = f(src, 0, size, (calis_num *) malloc(size * _calis_size),
	  cmp, 1, 1);
  if (size <= cacheSize)
  { ret = f(src, 0, size, dst, cmp, 2, 0);
    if (ret != dst) memcpy(dst, ret, size * _calis_size);
    free(src); return; }
  /* whether the sorted sublist ends up in src or dst depends on f and
     cacheSize, which are variable; we refer to sorted list with ret */
  ret = f(src, 0, cacheSize, dst, cmp, 2, 0);
  last = size - size % cacheSize;
  for (i = cacheSize; i < last; i += cacheSize)
    f(src, i, cacheSize, dst, cmp, 2, 0);
  tmpSize = _calis_min(cacheSize, size - last);
  if (tmpSize)
  /* last sublist's size differs from cacheSize, so sorted list might
     not end up in ret */
  { tmp = f(src, last, tmpSize, dst, cmp, 2, 0);
    if (tmp != ret)
      memcpy(ret + last, tmp + last, tmpSize * _calis_size); }
  ret = f(ret, 0, size, ret == src ? dst : src, cmp, cacheSize, 0);
  if (ret != dst) memcpy(dst, ret, size * _calis_size); free(src); }

/** example sorting algorithm--merge sort **/
void _calis_merge(
  calis_num src[], size_t aOffset, size_t bOffset, size_t bSize,
  calis_num *dst, calis_num(*cmp)(calis_num, calis_num))
/* Merge two contiguous sublists of SRC using comparison function CMP
   and output to DST starting at AOFFSET. */
{ calis_num i, j, *a = src + aOffset, *b = src + bOffset;
  size_t aSize = b - a; dst += aOffset;
  while (aSize && bSize)
  { i = *a; j = *b;
    if (cmp(i, j) > 0) { *dst = j; ++b; --bSize; }
    else               { *dst = i; ++a; --aSize; }
    ++dst; }
  if      (bSize) memcpy(dst, b, bSize * _calis_size);
  else if (aSize) memcpy(dst, a, aSize * _calis_size); }

calis_num *calis_mergesort(
  calis_num src[], size_t offset, size_t size, calis_num dst[],
  calis_num (*cmp)(calis_num, calis_num), size_t aSize, int iterations)
{ size_t end = offset + size, aOffset, bOffset, aaSize, bSize;
  calis_num *tmp;
  if (!iterations)
    iterations = (int) ceil(log((float) size / aSize) / log(2));
  for ( ; iterations > 0; --iterations)
  { aOffset = offset; bOffset = offset + aSize; aaSize = aSize * 2;
    while (bOffset < end)
    { _calis_merge(src, aOffset, bOffset,
		   _calis_min(aSize, end - bOffset), dst, cmp);
      aOffset += aaSize; bOffset += aaSize; }
    if (aOffset < end) memcpy(dst + aOffset, src + aOffset,
			      (end - aOffset) * _calis_size);
    aSize = aaSize; tmp = src; src = dst; dst = tmp; }
  return src; }

#endif
