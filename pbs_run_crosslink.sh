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

## start job from the directory it was submitted.
## NO PBS DIRECTIVES AFTER THIS!
cd $PBS_O_WORKDIR

## Variables should be defined in the environment of qsub and passed on using the -v option: -v VARIABLE="..."
if [ -z $CLA ]
then
echo "Variable CLA (command line arguments) was not defined. Please define, possibly empty."
else
./bin/Release/CrossLink "$CLA"
fi
