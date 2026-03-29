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
	echo Starting job on CPU1
	taskset 0x2 chrt -f 30 $*
}&

sleep 1
{
	echo Starting job on CPU2
	taskset 0x4 chrt -f 30 $*
}&

sleep 1
{
	echo Starting job on CPU3
	taskset 0x8 chrt -f 30 $*
}&

wait

