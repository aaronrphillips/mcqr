#!/bin/bash

##SBATCH -A BEDSTONE2
##SBATCH -t 59

#SBATCH -n 16
#SBATCH -N 2

tasks=`cat $0 | egrep -e "^#SBATCH [-]n" | awk '{print $NF}'`
nodes=`cat $0 | egrep -e "^#SBATCH [-]N" | awk '{print $NF}'`
exclude=`cat $0 | egrep -e "^#SBATCH [-]x" | awk '{print $NF}'`

host=`hostname | sed -e "s/[.].*$//"`

#olympus
module load gcc/4.8.2
export LD_LIBRARY_PATH=/pic/projects/mlstones/zeromq/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=../external/hyperic-sigar-1.6.4/sigar-bin/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/usr/lib:${LD_LIBRARY_PATH}

#bedstone
export LD_LIBRARY_PATH=/lustre/downloads/hyperic-sigar-1.6.4/sigar-bin/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/lustre/local/gcc_4_8_2/lib64:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/lustre/zeromq/lib:${LD_LIBRARY_PATH}

cmd=""
if [ x`echo $SLURM_JOB_ID` != "x" ]; then
  cmd="srun --no-kill --wait=604800 -n $tasks -N $nodes "
fi

sigar_lib="../external/hyperic-sigar-1.6.4/sigar-bin/lib/"
export DYLD_LIBRARY_PATH=$sigar_lib:$DYLD_LIBRARY_PATH
export DYLD_FALLBACK_LIBRARY_PATH=$sigar_lib:$DYLD_FALLBACK_LIBRARY_PATH

echo -n "START: "
date
$cmd ./mcqr -V -t test.tasks -c 8 -l 80 -m 90 -w 0.5  
echo -n "STOP:  "
date
