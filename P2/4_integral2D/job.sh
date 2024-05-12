#!/bin/bash
#SBATCH -J mpi_integral
#SBATCH --output=integral_%j.out
#SBATCH --error=integral_%j.err
#SBATCH -p ippd-cpu
#SBATCH --time=00:02:00
#SBATCH --nodes=1
#SBATCH --ntasks=1 #request MPI processes
#SBATCH --cpus-per-task=2 #threads OpMp

module load GCC/10.2.0
module load OpenMPI/4.1.2-GCC-10.2.0-with-slurm

export OMP_NUM_THREADS=2

make >> make.out || exit 1

mpirun -n 1 ./integral2D_par surface_10_10.dat 1000 1000
