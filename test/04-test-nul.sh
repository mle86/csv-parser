#!/bin/sh
. $(dirname "$0")/init.sh

# What happens if the input contains NUL bytes?

define expectedOutput <<-EOT
	[["n1","\\0n2"]
	,["n3","\\0n4"]]
EOT

assertCmdEq "$CSV -ij < $SAMPLE/nul.csv" "$expectedOutput" \
	"csv did not handle input NUL bytes correctly!"


define expectedOutput_j <<-EOT
	[{"n1":"n3","\\0n2":"\\0n4"}]
EOT
define expectedOutput_Jcompact <<-EOT
	{"columns": ["n1","\\0n2"],
	"lines": [
	 ["n3","\\0n4"]]}
EOT
define expectedOutput_m <<-EOT
	n1: n3
	\\0n2: \\0n4
EOT

assertCmdEq "$CSV -aj < $SAMPLE/nul.csv" "$expectedOutput_j" \
	"csv -j did not handle NUL bytes in column names correctly!"
assertCmdEq "$CSV -aJ < $SAMPLE/nul.csv" "$expectedOutput_Jcompact" \
	"csv -J did not handle NUL bytes in column names correctly!"
assertCmdEq "$CSV -am < $SAMPLE/nul.csv" "$expectedOutput_m" \
	"csv -m did not handle NUL bytes in column names correctly!"


success
