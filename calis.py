#!/usr/bin/python

# Python prototype of Cache-Aware Local Iterative Sort(libcalis)
# Author: Anseok Joo (anseok at gmail dot com)
# Public Domain

## customisables #######################################################
CACHE_SIZE = 4

## functions ###########################################################
def copy(src, srcOffset, size, dst, dstOffset):
  'copy sublist of size SIZE from SRC at SRCOFFSET to DST at DSTOFFSET'
  for i in xrange(size): dst[dstOffset + i] = src[srcOffset + i]

def sort(src, f, cacheSize, comp = cmp):
  '''sort list SRC with sorting algorithm F using comparison function
COMP assuming CPU cache size CACHESIZE'''
  size, dst = len(src), [0 for _ in src]
  if size <= cacheSize: return f(src, 0, size, dst, comp)
  # whether the sorted sublist ends up in SRC or DST depends on F and
  # CACHESIZE, which is variable; so we refer to sorted list with RET
  ret = f(src, 0, cacheSize, dst, comp)
  last = size - size % cacheSize
  for i in xrange(cacheSize, last, cacheSize):
    f(src, i, cacheSize, dst, comp)
  tmpSize = min(cacheSize, size - last)
  if tmpSize:
    # last sublist's size differs from CACHESIZE, so sorted list might
    # not end up in RET
    tmp = f(src, last, tmpSize, dst, comp)
    if tmp != ret: copy(tmp, last, tmpSize, ret, last)
  if ret != src: src, dst = dst, src
  return f(src, 0, size, dst, comp, cacheSize)

## example sorting algorithm--merge sort ###############################
def merge(src, aOffset, bOffset, bSize, dst, comp):
  '''merge two contiguous sublists of SRC using comparison function COMP
and output to DST starting at AOFFSET'''
  dstOffset, aSize = aOffset, bOffset - aOffset
  while aSize and bSize:
    i, j = src[aOffset], src[bOffset]
    if comp(i, j) > 0: dst[dstOffset] = j; bOffset += 1; bSize -= 1
    else:              dst[dstOffset] = i; aOffset += 1; aSize -= 1
    dstOffset += 1
  if bSize:   copy(src, bOffset, bSize, dst, dstOffset)
  elif aSize: copy(src, aOffset, aSize, dst, dstOffset)

def mergesort(src, offset, size, dst, comp, aSize = 1):
  '''the merge sort algorithm; uses comparison function CMP; ASIZE is
the smallest block size to consider'''
  end = offset + size
  while aSize < size:
    aOffset, bOffset, aaSize = offset, offset + aSize, aSize * 2
    while bOffset < end:
      bSize = min(aSize, end - bOffset)
      merge(src, aOffset, bOffset, bSize, dst, comp)
      aOffset += aaSize; bOffset += aaSize
    if aOffset < end: copy(src, aOffset, end - aOffset, dst, aOffset)
    aSize = aaSize; src, dst = dst, src
  return src

## run #################################################################
def main():
  n = 21
  src = range(n); random.shuffle(src); dst = [0 for _ in src]
  print sort(src, mergesort, CACHE_SIZE, cmp)

if __name__ == '__main__': import random; main()

# eof
