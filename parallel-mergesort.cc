/**
 *  \file mergesort.cc
 *
 *  \brief Implement your mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sort.hh"




// Conduct a binary search for x in T[p..r]
// Return the index where x is found,
// or return the index of the first number
// in the range that is > x.
int binary_search(int T[], int p, int r, int x)
{
  int a = p;
  int b = r;
  int c = (a + b) / 2;
  while (a < b) {
    if (T[c] == x)
      return c;
    if (T[c] < x)
      a = c + 1;
    else
      b = c;
    c = (a + b) / 2;
  }
  if (T[c] > x)
    return c;
  else
    return c + 1;
}

void merge(int T[], int p1, int r1, int p2, int r2, int A[], int q)
{
  int a = p1;
  int b = p2;
  int c = q;
  while (a <= r1 && b <= r2) {
    if (T[a] < T[b])
      A[c++] = T[a++];
    else
      A[c++] = T[b++];
  }
  while (a <= r1) {
    A[c++] = T[a++];
  }
  while (b <= r2) {
    A[c++] = T[b++];
  }
}

void parallel_merge(int A[], int p, int q, int r)
{
  int n = r - p + 1;
  int* T = new int[n];
  int i;
#pragma omp parallel for shared(A,p,n,T) private(i)
  for (i = 0; i < n; i++)
    T[i] = A[i + p];

  int p1 = 0;
  int r1 = q - p;
  int q1 = (p1 + r1) / 2;
  int p2 = q - p + 1;
  int r2 = r - p;
  int q2 = binary_search(T, p2, r2, T[q1]);
  int p3 = p;
  int q3 = p3 + (q1 - p1) + (q2 - p2);
  A[q3] = T[q1];
#pragma omp parallel sections shared(A,p1,r1,q1,p2,r2,q2,p3,q3)
  {
#pragma omp section
    merge(T, p1, q1 - 1, p2, q2 - 1, A, p3);
#pragma omp section
    merge(T, q1 + 1, r1, q2, r2, A, q3 + 1);
  }

  delete[] T;
}

void parallel_merge_sort(int A[], int p, int r, int level)
{
  if (level > 4) // Revert to sequential after 4 levels
    quicksort(A, p, r); // This limits the thread count
  else {
    int q = (p + r) / 2;
#pragma omp parallel sections shared(A,p,r,q)
    {
#pragma omp section
      parallel_merge_sort(A, p, q, level + 1);
#pragma omp section
      parallel_merge_sort(A, q + 1, r, level + 1);
    }
    parallel_merge(A, p, q, r);
  }
}





void
mySort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
}
/* eof */
