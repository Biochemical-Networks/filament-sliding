#!/bin/bash

#PBS -S /bin/bash

## set the number of nodes and processes per node
#PBS ‐l nodes=1:ppn=1

## set queue name
#PBS -q test

## set max wallclock time
#PBS -l walltime=100:00:00

## set email adress
#PBS -M h.wierenga@amolf.nl

## start job from the directory from which it was submitted.
## NO PBS DIRECTIVES AFTER THIS!
cd $PBS_O_WORKDIR

## Variables should be defined in the environment of qsub in the single environment variable CLA, via the -v option: -v CLA="...".
## e.g.: qsub pbs_run_crosslink.sh -v CLA="-gv 10 -np 20"
if [ -z $CLA ]
then
echo "Variable CLA (command line arguments) was not defined. Please define, possibly empty."
else
./bin/Release/CrossLink "$CLA"
fi
