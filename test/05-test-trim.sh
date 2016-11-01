#!/bin/sh
. $(dirname "$0")/init.sh

# The --trim options remove unwanted whitespace from the input.
# This tests verifies that trimming works correctly and does not remove any data.

# This test depends on test-noeol and test-whitespace.

define expectedOutput <<-EOT
	[["   c1 "," c2   "]
	,[" v1 "," v2 "]]
EOT
define expectedOutput_trim <<-EOT
	[["c1 "," c2"]
	,["v1 "," v2"]]
EOT
define expectedOutput_trim_q <<-EOT
	[[" c1 "," c2 "]
	,[" v1 "," v2 "]]
EOT

# Make sure that an explicit --trim=none does not remove any whitespace,
# neither in LF-terminated lines nor in a raw EOL line:
assertCmdEq "$CSV -aij --trim=none < $SAMPLE/trim-noeol.csv" "$expectedOutput" \
	"csv --trim=none produced incorrect output reading trim-noeol.csv!"

# Make sure that --trim=none is the default if there's no --trim option at all:
assertCmdEq "$CSV -aij < $SAMPLE/trim-noeol.csv" "$expectedOutput" \
	"csv without --trim option produced incorrect output reading trim-noeol.csv!"

# With --trim=lines, we'll expect the program to remove leading and trailing whitespace from every line.
assertCmdEq "$CSV -aij --trim=lines < $SAMPLE/trim-noeol.csv" "$expectedOutput_trim" \
	"csv --trim=lines produced incorrect output reading trim-noeol.csv!"
# Also, a simple --trim option (without MODE) should mean --trim=lines as well and not corrupt other options.
assertCmdEq "$CSV --trim -aij < $SAMPLE/trim-noeol.csv" "$expectedOutput_trim" \
	"csv --trim (without trim MODE) produced incorrect output reading trim-noeol.csv!"

# Special case:
# Quoted fields can contain whitespace, and it should not be removed.
# Of course, the field can still be preceded/trailed by whitespace outside the quoting characters, and that should get removed:
# >>  " field "  << should get converted to >> field <<.
assertCmdEq "$CSV -aij --trim=lines < $SAMPLE/trim-q-noeol.csv" "$expectedOutput_trim_q" \
	"csv --trim=lines produced incorrect output reading trim-q-noeol.csv!"

# What about an input file containing only whitespace and empty fields?
define expectedOutput_ws <<-EOT
	[["",""]
	,["",""]
	,[""]
	,[""]
	,[""]
	,[""]
	,[""]]
EOT
assertCmdEq "$CSV -aij --trim=lines < $SAMPLE/whitespace.csv" "$expectedOutput_ws" \
	"csv --trim=lines had problems with whitespace.csv!"


success
