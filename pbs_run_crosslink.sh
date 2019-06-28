#!/bin/bash

#PBS -S /bin/bash

## Set the job name
#PBS -N CrossLink

## set the number of nodes and processes per node
#PBS ‐l nodes=1:ppn=1

## set queue name
#PBS -q test

## set max wallclock time
#PBS -l walltime=100:00:00

## set memory 
#PBS -l mem=256m

## set email adress
#PBS -M h.wierenga@amolf.nl

## Write std_out and std_err to out directory, with these file names:
#PBS -o out/"CrossLink.${PBS_JOBID%".head.hollandia.amolf.nl"}.out"
#PBS -e out/"CrossLink.${PBS_JOBID%".head.hollandia.amolf.nl"}.err"

## start job from the directory from which it was submitted.
## NO PBS DIRECTIVES AFTER THIS!
cd $PBS_O_WORKDIR

## Variables should be defined in the environment of qsub in the single environment variable CLA, via the -v option: -v CLA="...".
## e.g.: qsub pbs_run_crosslink.sh -v CLA="-gv 10 -np 20"
## Test whether CLA is defined. Don't just test if it is empty, because that is also allowed.
## https://stackoverflow.com/questions/3601515/how-to-check-if-a-variable-is-set-in-bash:
if [ -z "${CLA+x}" ]
then
>&2 echo "Variable CLA (command line arguments) was not defined. Please define, possibly empty."
else
## Don't use quotes in the variable expansion of CLA, since it can contain multiple words and each should be interpreted by CrossLink as a separate command line argument,
## and not as a single big argument (containing spaces), which would be the case if we used ' ./bin/Release/CrossLink "$CLA" '.
./bin/Release/CrossLink $CLA
fi
