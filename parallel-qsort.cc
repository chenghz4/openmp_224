/**
 *  \file parallel-qsort.cc
 *
 *  \brief Implement your parallel quicksort algorithm in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "sort.hh"
#include <math.h> 
#include <vector>
#include <iostream>
#include<string.h>
using namespace std;



int partition_short (keytype pivot, int N, keytype* A)
{
  int k = 0;
  int i;
  for (i = 0; i < N; ++i) {
      /* Invariant:
      * - A[0:(k-1)] <= pivot; and
      * - A[k:(i-1)] > pivot
      */
     
     const int ai = A[i];
     if (ai <= pivot) {
        /* Swap A[i] and A[k] */
      int ak = A[k];
       A[k++] = ai;
       A[i] = ak;
     }        
  }
  return k;
}
/**
 *   Given a pivot value, this routine partitions a given input array
 *   into two sets: the set A_le, which consists of all elements less
 *   than or equal to the pivot, and the set A_gt, which consists of
 *   all elements strictly greater than the pivot.
 *      
 *   This routine overwrites the original input array with the
 *   partitioned output. It also returns the index n_le such that
 *   (A[0:(k-1)] == A_le) and (A[k:(N-1)] == A_gt).
 */
int partition (keytype pivot, int N, keytype* A,keytype *B)
{
  int k = 0;
  int i;
  int G=N/100;
  if(N<=10000) k=partition_short(pivot,N,A);

  else{
   
    //keytype* B = newKeys (N);;
    int number=floor(N/G);
    int ss=N-(number-1)*G;
    int flag[number];
    int flag_fix[number];
    int flag_fix_big[number];
   // #pragma omp taskgroup
    //{
    #pragma omp taskloop shared (flag,A)
    for (int j = 0; j<number; ++j) {
      if(j==number-1) flag[j]=partition_short(pivot,ss,A+j*G);
      else flag[j]=partition_short(pivot,G,A+j*G);
    }


   // #pragma omp taskwait
    //}

    for(int j=0;j<number;j++){
      if(j==0) flag_fix[j]=flag[0];
      else flag_fix[j]=flag_fix[j-1]+flag[j];
    }
    k=flag_fix[number-1];

    
    for(int j=0;j<number;j++){
      if(j==0) {
        if(number!=1)flag_fix_big[j]=G+k-flag[j];
        else flag_fix_big[j]=N;
      }
      else{
        if(j==number-1) flag_fix_big[j]=N;
        else flag_fix_big[j]=flag_fix_big[j-1]+G-flag[j];
      }
    }
  
   
    #pragma omp taskloop shared(flag,flag_fix,B,A) 
    for (int j = 0; j<number; ++j) {
    
      if(j==0) {
        memcpy(B,A,flag_fix[j]*sizeof(unsigned long));
      }
      else{
        memcpy(B+flag_fix[j-1],A+G*j,flag[j]*sizeof(unsigned long));
      }
    }



    //#pragma omp taskwait

    #pragma omp taskloop shared(flag,flag_fix,flag_fix_big,B,A) ///change the array
    
    for (int j = 0; j<number; ++j) {
      if(j==0) {
        memcpy(B+k,A+flag[j],(flag_fix_big[j]-k)*sizeof(unsigned long));
      }
      else{
        if(j==number-1) memcpy(B+flag_fix_big[j-1],A+G*j+flag[j],(ss-flag[j])*sizeof(unsigned long));
        else memcpy(B+flag_fix_big[j-1],A+G*j+flag[j],(G-flag[j])*sizeof(unsigned long));
      }
    }
    //#pragma omp taskwait
   // memcpy(A,B,N*sizeof(unsigned long));
    
  
    #pragma omp taskloop shared(A,B)
    for (int j = 0; j<number; ++j) {
      if(j==number-1) memcpy(A+j*G,B+j*G,ss*sizeof(unsigned long));
      else memcpy(A+j*G,B+j*G,G*sizeof(unsigned long));
    }
   
    

    //free (B);
  }
  return k;
}






void quickSort (int N, keytype* A,keytype* B)
{
  
  const int G = 1024; /* base case size, a tuning parameter */
  if (N <= G)
    sequentialSort (N, A);
  else {
    // Choose pivot at random
    keytype pivot = A[rand () % N];
    //cout<<"total number:"<<N<<endl;
    // Partition around the pivot. Upon completion, n_less, n_equal,
    // and n_greater should each be the number of keys less than,
    // equal to, or greater than the pivot, respectively. Moreover, the array



    int n_le = partition_short (pivot, N, A);

    #pragma omp task
    quickSort (n_le, A,B);
    quickSort (N-n_le, A + n_le,B+n_le);
    #pragma omp taskwait

    
     
  }
}

void mySort (int N, keytype* A,keytype *B)
{
  
  
  quickSort (N, A,B);
}

/* eof */
