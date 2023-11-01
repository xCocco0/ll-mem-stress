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
	taskset 0x1 chrt -f 50 $1
}&

sleep 1
{
	echo Starting job on CPU1
	taskset 0x2 chrt -f 50 $1
}&

sleep 1
{
	echo Starting job on CPU2
	taskset 0x4 chrt -f 50 $1
}&

wait

