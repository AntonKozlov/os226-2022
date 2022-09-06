#!/bin/bash

B=$(readlink -f $(dirname $0))

runmain() {
	( cd $B/.. ; ./target/debug/os226-2022 )
}

. $B/run.sh