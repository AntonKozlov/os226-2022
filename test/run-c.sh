#!/bin/bash

B=$(readlink -f $(dirname $0))

runmain() {
	( cd $B/.. ; ./main )
}

. $B/run.sh