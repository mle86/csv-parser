#!/bin/sh
. $(dirname "$0")/init.sh

# This test checks what happens when reading a file
# that contains only empty fields and whitespace.
# Without --trim, the whitespace should be copied as-is.

define expectedOutput <<-EOT
	[["",""]
	,[" "," "]
	,[""]
	,[" "]
	,["  "]
	,["   "]
	,["\\t"]]
EOT

assertCmdEq "$CSV -aij < $SAMPLE/whitespace.csv" "$expectedOutput" \
	"csv produced unexpected output reading whitespace.csv!"

success
