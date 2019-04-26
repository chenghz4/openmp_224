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

int partition (keytype pivot, int N, keytype* A,keytype *B)
{
  int k = 0;
  int i;
  int G=N/100;
  if(N<=10000) k=partition_short(pivot,N,A);

  else{
    int number=floor(N/G);
    int ss=N-(number-1)*G;
    int flag[number];
    int flag_fix[number];
    int flag_fix_big[number];
   
    #pragma omp taskloop shared (flag,A)
    for (int j = 0; j<number-1; ++j) {
     
      flag[j]=partition_short(pivot,G,A+j*G);
    }
    flag[number-1]=partition_short(pivot,ss,A+(number-1)*G);


/////////sequencial///////////////////////////////
    flag_fix[0]=flag[0];
    for(int j=1;j<number;j++){
      flag_fix[j]=flag_fix[j-1]+flag[j];
    }
    k=flag_fix[number-1];

    
    flag_fix_big[0]=G+k-flag[0];
    for(int j=1;j<number-1;j++){
      flag_fix_big[j]=flag_fix_big[j-1]+G-flag[j];
      
    }
    flag_fix_big[number-1]=N;
////////////////////////////////////////////////

    memcpy(B,A,flag_fix[0]*sizeof(unsigned long));    //to make the branch miss lower 
    #pragma omp taskloop shared(flag,flag_fix,B,A) 
    for (int j = 1; j<number; ++j) {
        memcpy(B+flag_fix[j-1],A+G*j,flag[j]*sizeof(unsigned long));
    }
    

    memcpy(B+k,A+flag[0],(flag_fix_big[0]-k)*sizeof(unsigned long)); //to make the branch miss lower 
    #pragma omp taskloop shared(flag,flag_fix,flag_fix_big,B,A) ///change the array
    for (int j = 1; j<number; ++j) {
      
      if(j==number-1) memcpy(B+flag_fix_big[j-1],A+G*j+flag[j],(ss-flag[j])*sizeof(unsigned long));
      else memcpy(B+flag_fix_big[j-1],A+G*j+flag[j],(G-flag[j])*sizeof(unsigned long));
    
    }
   
    
  
    #pragma omp taskloop shared(A,B)
    for (int j = 0; j<number-1; ++j) {
      
     memcpy(A+j*G,B+j*G,G*sizeof(unsigned long));
    }
    memcpy(A+(number-1)*G,B+(number-1)*G,ss*sizeof(unsigned long)); //to make the branch miss lower 

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
    //cout<<"number"<<omp_get_thread_num()<<endl;


    int n_le = partition (pivot, N, A,B);

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
