#!/bin/bash

for (( NLINKERS=6; NLINKERS<=20; ++NLINKERS ))
do
qsub pbs_run_crosslink.sh -v CLA="-n coopE5N${NLINKERS} -np ${NLINKERS}"
done
