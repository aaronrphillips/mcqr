#!/bin/bash

#SBATCH -A MINT
#SBATCH -t 5
#SBATCH -n 3
#SBATCH -N 3
#SBATCH --partition=short

./mcqr -t test.tasks -V -H `../utilities/get_nodelist_slurm.sh $SLURM_JOBID` -L logdir -c 1
