/***********************************************************************
A program exemplifying calis.h usage
Author: Anseok Joo (anseok at gmail dot com)
Public domain

To compile on Unix/Linux, run "cc calis_example.c -lm -o calis_example".
***********************************************************************/

#include <stdio.h>
#include <time.h>  /* used for setting random seed */

/* First, typedef or #define calis_num.  It can be any comparable (and
   hence sortable) type, such as int, float, char *, etc.  This must be
   done before including calis.h. */
typedef int calis_num;
#include "calis.h"

const size_t cacheSize = 8;
/* cacheSize is the size of used CPU cache in bytes.  It does not
   have to be a power of 2.  If it is an odd number, cacheSize-1 bytes
   are used.  Setting cacheSize to the entire CPU cache might not be
   optimal since other concurrently running processes might compete for
   the cache space. */

void shuffle(calis_num a[], size_t n)
/* Randomly shuffle integers from 0 to n-1 and store them in array a. */
{ int i = 0, j;
  for ( ; i < n; ++i) a[i] = -1;
  for (i = 0; i < n; ++i)
  { do j = rand() % n; while (a[j] >= 0);
    a[j] = i; } }

void printArray(calis_num a[], size_t n)
/* Pretty-print an array a of size n. */
{ int i = 0; printf("[");
  for (--n ; i < n; ++i) printf("%d, ", a[i]);
  printf("%d]\n", a[n]); }

int main(int argc, char **argv)
{ size_t n = 21, size = n * sizeof (calis_num);
  calis_num *src = (calis_num *) malloc(size),
            *dst = (calis_num *) malloc(size);
  srand((unsigned int) time(NULL));  /* set random seed */
  shuffle(src, n);
  /* below is how you invoke calis_sort */
  calis_sort(
    src,             /* array to be sorted */
    n,               /* size of src */
    dst,             /* array where sorted list will be stored; if we
                        put src here, the sort would be destructive */
    calis_mergesort, /* sorting algorithm; can be any divide-and-conquer
			algorithm with the same function signature as
			calis_mergesort; see calis.h line 37 */
    cacheSize,       /* size of CPU cache to use */
    calis_cmp);      /* comparison function; see calis.h line 32 */
  /* now sorted list should appear in dst */
  printArray(src, n); printArray(dst, n);
  free(src); free(dst); return 0; }
