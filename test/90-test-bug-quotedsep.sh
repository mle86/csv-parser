#!/bin/sh
. $(dirname "$0")/init.sh

# If the first line's first field is quoted
# and contains a character which could be a separator,
# the program must keep looking for the separator.

define input <<-EOT
	"very""long,field";second-field
EOT
export input
define expectedOutput <<-EOT
	[["very\\"long,field","second-field"]]
EOT

assertCmdEq "echo \"\$input\" | $CSV -ije" "$expectedOutput"

success
