#!/bin/bash
. $(dirname "$0")/init.sh

# All tests so far have assumed a file with a separator character.
# Check the handling of input files without a separator character,
# with and without the "-d none" option.

assertCmd "$CSV -am < $HELPER/nosep.csv" $EXIT_NOSEP \
	"csv exited with an unexpected exit code while reading nosep.csv!"

expectedOutput="header-line-without-separator: cell1;cell2|cell3${TAB}cell4,cell5"

assertCmdEq "$CSV -am -d none < $HELPER/nosep.csv" "$expectedOutput" \
	"\"csv -d none\" failed while reading nosep.csv!"


# The "-d none" option also forces a file with regular separator characters
# to be interpreted as single-column.

define expectedOutput <<-EOT
	[{"l1c1;l1c2;l1c3;l1c4;l1c5":"l2c1;l2c2;l2c3;l2c4;l2c5"}
	,{"l1c1;l1c2;l1c3;l1c4;l1c5":"l3c1;l3c2;l3c3;l3c4;l3c5"}]
EOT

assertCmdEq "$CSV -aj -d none < $HELPER/sample1.csv" "$expectedOutput" \
	"\"csv -d none\" did not ignore separator characters in sample1.csv!"

success
