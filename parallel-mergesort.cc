/**
 *  \file mergesort.cc
 *
 *  \brief Implement your mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "sort.hh"
#include <omp.h>
#include <math.h> 
#include<string.h>
#include<iostream>
using namespace std;
int number;

int binary_search(keytype* A, int N, keytype x)
{
  int a = 0;
  int b = N;
  int c = (a + b) / 2;
  while (a <b) {
    if (A[c] == x)  return c;
    if (A[c] < x)  a = c + 1;
    else    b = c;
    c = (a + b) / 2;
  }

  return c;
}
  

void merge(keytype *A, int p1, int r1, int p2, int r2, keytype*B) //seqential merge
{
  int a = p1;
  int b = p2;
  int c = 0;
  while (a <= r1 && b <= r2) {
    if (A[a] < A[b])  B[c++] = A[a++];
    else   B[c++] = A[b++];
  }
  while (a <= r1) {
    B[c++] = A[a++];
  }
  while (b <= r2) {
    B[c++] = A[b++];
  }
}


void parallel_merge(keytype *A, int s1,int r1, int s2, int r2,keytype*B) //[s1,r1],[s2,r2]
{
  const int G = number/1000;
  int num=r1-s1+1;
  int num1=r2+r1-s1-s2;
  if(num1<=G){
  merge(A,s1,r1,s2,r2,B);
  }
  else{
  int temp=rand () % (num)+s1;
  keytype pivot = A[temp];
  int q2 = binary_search(A+s2, r2-s2+1, pivot)+s2;

  #pragma omp task
  parallel_merge(A,s1,temp-1,s2,q2-1,B);
  parallel_merge(A,temp,r1,q2,r2,B+temp-s1+q2-s2);
  //merge(A,s1,temp-1,s2,q2-1,B);
  //merge(A,temp,r1,q2,r2,B+temp-s1+q2-s2);
  #pragma omp taskwait
  }
}







bool parallel_merge_sort(keytype* A, keytype* B,int N)
{
  bool j;
  const int G = 1024; /* base case size, a tuning parameter */
  if (N <= G){
    sequentialSort (N, A);
    return false;
  }
  else {
    int index = floor((N-1)/2);
   
    #pragma omp task
    parallel_merge_sort(A,B,index);
    j=parallel_merge_sort(A+index,B+index,N-index);
    #pragma omp taskwait
    //merge(A,0,index-1,index,N-1,B);
    //memcpy(A,B,N*sizeof(unsigned long));
    //parallel_merge(A,0,index-1,index, N-1, B);
    if(!j) parallel_merge(A,0,index-1,index, N-1, B);
    else parallel_merge(B,0,index-1,index, N-1, A);
    
    return (!j);
   
  }
}


void mySort (int N, keytype* A,keytype*B)
{
  number=N;
  bool j=parallel_merge_sort(A,B,N);
  if(j) memcpy(A,B,N*sizeof(unsigned long));
  /* Lucky you, you get to start from scratch */
}
/* eof */
