#!/bin/sh
. $(dirname "$0")/init.sh

# All tests so far had input with a trailing linebreak on the last line.
# An otherwise identical file without that final linebreak should
# result in the same output:

define expectedOutput <<-EOT
	[["c1","c2"]
	,["v1","v2"]]
EOT

assertCmdEq "( cat $SAMPLE/noeol.csv ; echo ) | $CSV -ij" "$expectedOutput" \
	"csv produced unexpected output reading noeol.csv with an added linebreak!"

assertCmdEq "( cat $SAMPLE/noeol.csv        ) | $CSV -ij" "$expectedOutput" \
	"csv produced incorrect output reading noeol.csv, which has no trailing linebreak!"

success
