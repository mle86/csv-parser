#!/bin/bash
. $(dirname "$0")/init.sh

# Verify that the program aborts when encountering a malformed input file:

assertCmd "$CSV -a < $SAMPLE/nosep.csv" $EXIT_NOSEP \
	"csv failed while reading nosep.csv!"

# Now try it again, with the -e option:

expectedOutput="header-line-without-separator: cell1;cell2|cell3${TAB}cell4,cell5"

assertCmdEq "$CSV -a -e < $SAMPLE/nosep.csv" "$expectedOutput" \
	"csv -e failed while reading nosep.csv!"

success
