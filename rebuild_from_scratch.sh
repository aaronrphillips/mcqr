#!/bin/bash

. $HOME/.bashrc

ncpu=`grep processor /proc/cpuinfo | wc -l`
host=`hostname | sed -e "s/[.].*$//"`

#mac
export ZMQ_ROOT=/usr/local/Cellar/zeromq/4.0.3
ncpu="4"

#bedstone
if [ "$host" = "bedstone" ]; then
  export ZMQ_ROOT=/lustre/zeromq 
  export GNU_ROOT=/lustre/local/gcc_4.8.2
  export LD_LIBRARY_PATH=/lustre/lib:$LD_LIBRARY_PATH
fi

#olympus
if [ "$host" = "olympus" ]; then
#  export ZMQ_ROOT=/pic/projects/mlstones/zeromq
  module load zeromq/4.0.4
  ZMQ_ROOT=/share/apps/zeromq/4.0.4
  export GNU_ROOT=/share/apps/gcc/4.8.2
  export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH
fi


export GNU_VER=`echo "$GNU_ROOT" | awk -F'/' '{print $NF}'`
echo "GNU:$GNU_VER"
export LD_LIBRARY_PATH=$GNU_ROOT/lib64:$LD_LIBRARY_PATH
export PATH=$GNU_ROOT/bin:$PATH

module load gcc/$GNU_VER
module load autoconf

echo "###############"
echo "# Clean       #"
echo "###############"
#make distclean
make clean
rm -rf autom4te.cache
rm -f  config.log config.status Makefile
rm -f  bin/*

echo "###############"
echo "# autoreconf  #"
echo "###############"
autoreconf -fi 
automake
#autoreconf --install --force 

echo "###############"
echo "# configure   #"
echo "###############"
#LDFLAGS="$LDFLAGS -L/usr/lib64 -L$GNU_ROOT/lib64 -static" \
#CXXFLAGS="$CXXFLAGS -g -static -O0 -I$GNU_ROOT/include/c++/4.8.0 -I$BOOST_ROOT/include" \
LDFLAGS="$LDFLAGS -L/usr/lib64 -L$GNU_ROOT/lib64 " \
#CXXFLAGS="$CXXFLAGS -std=c++11 -g -O3 -I$GNU_ROOT/include/c++/4.8.0 -I$BOOST_ROOT/include" \
CXXFLAGS="$CXXFLAGS  -std=c++11 -g -O3 -I$GNU_ROOT/include/c++/$GNU_VER " \
./configure --enable-maintainer-mode --prefix=`pwd` --with-zeromq=$ZMQ_ROOT

echo "###############"
echo "# make        #"
echo "###############"
#make -j $ncpu && make -j $ncpu release && make -j $ncpu check && make -j $ncpu install
#LDFLAGS="$LDFLAGS -L/usr/lib64 -L$GNU_ROOT/lib64 -static" \
make -j $ncpu && make -j $ncpu install
#cd test
#make -j $ncpu && make check
