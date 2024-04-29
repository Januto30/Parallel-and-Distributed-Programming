#!/bin/bash
#SBATCH -J mpi_comms
#SBATCH --output=out_%j.out
#SBATCH --error=out_%j.err
#SBATCH -p ippd-cpu
#SBATCH --time=00:02:00
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=1

module load GCC/10.2.0
module load OpenMPI/4.1.2-GCC-10.2.0-with-slurm

export OMP_NUM_THREADS=1

make >> make.out || exit 1 # Exit if make fails

mpirun -n 4 mpi_comms
