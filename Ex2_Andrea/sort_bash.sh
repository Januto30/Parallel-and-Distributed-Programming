#!/bin/bash

#SBATCH --job-name=sort_name
#SBATCH -p ippd-cpu
#SBATCH --output=sort_%j.out
#SBATCH --error=sort_%j.err
#SBATCH --time=00:00:10
#SBATCH --ntasks=1
#SBATCH --nodes=1

make two 
//canviar segons num de cores (two,four,eight,sixteen)
./sort 200000
make clean

