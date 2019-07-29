#!/bin/bash

# A regular expression to check if it is an integer:
re='^[0-9]+$'

if [ "$#" -ne 3 ]
then
>&2 echo "Not the right number of arguments."
elif [ -z "$1" ]
then
>&2 echo "Run name is empty."
elif [ -z "$2" ] || [[ ! ($2 =~ $re) ]]
then
>&2 echo "Number of runs is empty or not a number."
elif [ -z "$3" ] || [[ ! ($3 =~ $re) ]]
then
>&2 echo "Initial run number is empty or not a number."
fi

RUN_NAME="$1"
NUMBER_RUNS="$2"
RUN_NUMBER_INIT="$3"

MAX=$(($NUMBER_RUNS + $RUN_NUMBER_INIT))

for ((i="$RUN_NUMBER_INIT";i<"$MAX";i++)); do
qsub pbs_run_crosslink.sh -v CLA="-n ${RUN_NAME}.${i}";
done
