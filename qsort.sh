#!/bin/bash
#$ -N quicksort
#$ -q pub8i
#$ -pe openmp 8
  ./quicksort 10000000
# eof