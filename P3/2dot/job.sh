#!/bin/bash
#SBATCH --job-name=dot
#SBATCH --output=%x_%j.out
#SBATCH --error=%x_%j.err
#SBATCH --partition=ippd-gpu
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:1
#SBATCH --time=00:00:15

module load NVHPC

make || exit 1
./dot.x 1000000
