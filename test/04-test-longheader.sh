#!/bin/bash
. $(dirname "$0")/init.sh

# The separator auto-detection only works on the very first input line;
# the man page says so.
# That means if the very first input field is a multi-line field,
# we'll have to specify the separator manually.
# This test checks that this corner case works as intended.

define expectedOutput <<-EOT
	[{"long\\nfirst\\nfield":"v1","second-field":"v2"}]
EOT

assertCmdEq "$CSV -j -b -d ';' < $SAMPLE/longheader.csv" "$expectedOutput" \
	"\"csv -jbd;\" did not read longheader.csv correctly!"

success
