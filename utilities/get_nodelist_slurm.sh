#!/bin/bash

jobid=$1
compressed_list=`scontrol show  JobId=$jobid | grep " NodeList" | awk -F = '{print $NF}'`
echo `scontrol show hostname $compressed_list | paste -d, -s`
