#!/bin/bash
. $(dirname "$0")/init.sh

# Verify that the program aborts when encountering a malformed input file:

assertCmd "$CSV -am < $SAMPLE/nosep.csv" $EXIT_NOSEP \
	"csv exited with an unexpected exit code while reading nosep.csv!"
assertCmd "$CSV -aj < $SAMPLE/malformed.csv" $EXIT_FORMAT \
	"csv exited with an unexpected exit code while reading malformed.csv!"

# Now try it again, with the -e option:

expectedOutput="header-line-without-separator: cell1;cell2|cell3${TAB}cell4,cell5"
assertCmdEq "$CSV -am -e < $SAMPLE/nosep.csv" "$expectedOutput" \
	"csv -e failed while reading nosep.csv!"

expectedOutput='[{"col1":"cell1","col2":"cell2 eof"}]'
assertCmdEq "$CSV -aj -e < $SAMPLE/malformed.csv" "$expectedOutput" \
	"csv -e failed while reading malformed.csv!"

success
