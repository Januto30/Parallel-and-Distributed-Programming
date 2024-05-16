#!/bin/bash
#SBATCH --job-name=cgcpuprf
#SBATCH --output=%x_%j.out
#SBATCH --error=%x_%j.err
#SBATCH --partition=ippd-gpu
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:1
#SBATCH --time=00:01:35


module load NVHPC

nvc -acc=host cg_cpu.c -g -o cg_cpu.x

nvprof --cpu-profiling on --cpu-profiling-mode flat ./cg_cpu.x
