#!/bin/sh

usage="Usage: $0 executable"

if [ "$#" -lt 1 ]; then
	echo $usage
	exit 1
fi

executable=$1

cleanup() {
	trap - INT
	trap - TERM
	trap - EXIT
	echo Cleaning up...
	pkill $(basename $executable)
	echo Done
}

trap "cleanup" INT TERM EXIT

{
	echo Starting job on CPU0
	chrt -d --sched-runtime 950000000 --sched-period 1000000000 --sched-deadline 0 0 $1
}&

sleep 1
{
	echo Starting job on CPU1
	chrt -d --sched-runtime 950000000 --sched-period 1000000000 --sched-deadline 0 0 $1
}&

sleep 1
{
	echo Starting job on CPU2
	chrt -d --sched-runtime 950000000 --sched-period 1000000000 --sched-deadline 0 0 $1
}&

wait

