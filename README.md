```
DISCLAIMER:

The software and materials was prepared as an account of work by AUTHORS
at the Pacific Northwest National Laboratories and sponsored by an agency
of the United States Government. Neither the United States Government nor
the United States Department of Energy, nor Battelle, nor any of their
employees, MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LEGAL
LIABILITY OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR USEFULNESS
OF ANY INFORMATION, APPARATUS, PRODUCT, SOFTWARE, OR PROCESS DISCLOSED,
OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS.

ACKNOWLEDGMENT:

This software and its documentation were produced with United States
Government support under Contract Number DE-AC05-76RL01830 awarded by
the United States Department of Energy. The United States Government
retains a paid-up non-exclusive, irrevocable worldwide license to
reproduce, prepare derivative works, perform publicly and display
publicly by or for the US Government, including the right to distribute
to other US Government contractors.
```

mcqueuer
========

Read the INSTALL file for installing the software on your system.


introduction
-------------

Mcqueuer is an extremely simple program. It takes a list of independant* commands
(anything that you would run from the command line), and runs them in parallel on
many processors on many nodes.
  * This is a subtle, but important point. Currently, mcqueuer assumes that your
    commands do not depend on one another, and that they can be run in any order.
    Future work would likely include mechanisms to introduce dependance.

the task file
-------------

The task file is simply a plain text file where each line represents a task (linux
command) the mcqueuer will farm out to a processor. These commands can be
identical, they can be slight variations of one another, or they can be completely
unrelated.

All mcqueuer assumes is:

  - Each line represents a command that can be executed on a node in your cluster.
  - Each command is independant (see above)

Finally, mcqueuer will generally perform better if you try to organize your task
file with the longest-running tasks first, although this is not a requirement.

comments
--------
Comments may be added to the task file by inserting a `#` character on the front
of a line. These lines will be ignored by mcqueuer.

example
--------

The following is an example of a valid (although useless) task file (assuming `some_program.sh` actually existed, of course)

```
echo "This is the first task!"
touch ~/somefile.txt
ls
./some_program.sh --some-argument 0.1 > someotherfile.txt
# This line is a comment!
date
```

controlling mcqueuer
--------------------
When you run mcqueuer on a cluster, it will distribute the tasks in the task file to different processors on different nodes. To do this, it uses 1 core* on each node as a "node controller". This controller is in charge of distributing tasks to the cores on its node, as well as ensuring that memory, load, and number of concurrent tasks are all "acceptable".

You as a user are in charge of defining what "acceptable" means. If your tasks take up lots of RAM, then you can set the `--max-memory` argument and limit how much of a machine's memory can be allocated before launching more jobs. If you only want 2 tasks to run per node, then just set `--max-cores` to 2. Mcqueuer will ensure that tasks are not launched when there is not enough memory, cores, or available load.**

You can also specify a "task wait time". This lets the controller know how many seconds to wait before executing another task on the node (even if all of the load constraints are within limits). This lets you give the programs that you're running time to "spin up", so that the load on each node can be accurately determined before launching more tasks.

*One node will actually have two processors dedicated to job management. 1
 processor will be the node controller, and one will be an overall "job
 controller", which distributes tasks to the job controllers on each node

**Another subtlety. Notice we don't claim that mcqueuer will ensure that you never use too much RAM, or that the load never gets too high. All we can do is check before we launch another task to make sure we're not violating the constraints. Because we don't care what commands you're running, we have no way of knowing if your program will decide to chew up 80% of the memory 30 minutes into execution. All we can do is make sure we don't launch another task after that's happened.

arguments
---------

```
short arg long arg              description
-------------------------------------------
-M        --set-master : specify the node name for the 'master'. If name matches $HOSTNAME, assign the 'master' role (localhost)
-W        --worker     : assign the 'worker' role
-C        --controller : assign the 'controller' role
-H        --host-list  : comma-separated list of node names on which to spin up controllers
-L        --log-dir    : directory where mcqr will write log files for node controllers and workers[.]
-h        --help       : show this help information
-v        --version    : show version information
-V        --verbose    : turn on verbose debugging [=0]
-t        --tasks      : task file (REQUIRED)
-p        --prog-file  : progress file - write progress to the specified text file
-c        --max-cores  : max number of worker cores per node, where 0 = unlimited [=0]
-l        --max-load   : max load on machine (% - as an integer), where 0 = unlimited. [=0]
-m        --max-memory : max memory usage (% - as an integer), where 0 = unlimited [=0]
-w        --task-wait  : minimum time (in seconds) between launching any two tasks on a node (allows fractional seconds) [=15.00]
-d        --temp-dir   : directory (on node or global FS) where mcqueuer can write temporary files [=/tmp]
```

Note that because `--max-load` and `--max-memory` are percentages, mcqueuer works well on asymmetric hardware. For example, if you specified `--max-memory=80`, and you ran one node that had 64GB of memory, and another that had 8GB of memory, mcqueuer would use limits of 51.2GB and 6.4GB, respectively.

The sbatch file
---------------
You can also control mcqueuer indirectly through the arguments in the sbatch file. For example, take a look at the following mcqueuer sbatch file:

```
!/bin/bash
#SBATCH -n 16
#SBATCH -N 2

./mcqr -c 4 -t tasks.txt -H node1,node2
```

This will launch 16 mcqueuer instances across 2 nodes – 13 workers, since there will be 2 node controllers and 1 job controller (see above) Since we specified `-c 4` on the command line, only 4 cores will be able to execute tasks at the same time on each node (8 overall). The other 5 workers will be sitting idle.

The `-H` option specifies a comma-separated list of node names that mcqr should launch node controllers on. **Note that this requires no-password SSH access to these nodes (likely using public/private keys).**

putting it all together
-----------------------

### task file

```
echo "This is the first task!"
touch ~/somefile.txt
ls
./some_program.sh --some-argument 0.1 > someotherfile.txt
# This line is a comment!
date
```

### sbatch file

Make sure to replace `[mcqueuer path]` and `[zeromq path]` with their respective absolute paths.

```
#!/bin/bash
#SBATCH -n 64
#SBATCH -N 2

module load gcc/4.8.2
export LD_LIBRARY_PATH=[sigar install]/sigar-bin/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=[zeromq path]/lib:${LD_LIBRARY_PATH}

./mcqr -t tasks.txt -V --max-cores=28 --max-load=75 --max-memory=90 --task-wait=5.5 --host-list=node1,node2
```
