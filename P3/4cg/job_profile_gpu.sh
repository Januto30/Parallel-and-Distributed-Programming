#!/bin/bash
#SBATCH --job-name=cggpuprf
#SBATCH --output=%x_%j.out
#SBATCH --error=%x_%j.err
#SBATCH --partition=ippd-gpu
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:1
##SBATCH --time=00:01:35

module load NVHPC

nvc -acc=gpu -Minfo=accel -g cg_gpu.c -o cg_gpu.x

nvprof ./cg_gpu.x
