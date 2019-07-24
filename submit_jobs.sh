#!/bin/bash

NUMBER_RUNS=16

for ((i=0;i<"$NUMBER_RUNS";i++)); do
qsub pbs_run_crosslink.sh -v CLA="";
sleep 2; # wait to make sure that the submitted program can create an input file before the next one is submitted
done