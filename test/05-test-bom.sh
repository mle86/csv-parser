#!/bin/sh
. $(dirname "$0")/init.sh

# So far, the test input never contained a BOM sequence,
# and if the tests were successful, the program does not remove any non-BOM prefixes.
# Now we check if the automatic BOM removal works correctly
# and can be turned off with the -M option.

# This test depends on test-limitskip.

BOM="﻿"  # utf-8 bom

#define input <<-EOT
#	${BOM}col1;col2
#	val1;val2
#	${BOM}val3;val4
#EOT

# The header line's BOM should have been stripped.
# The third line contains an extra BOM -- the program should not have touched it.
define expectedOutput <<-EOT
	[{"col1":"val1","col2":"val2"}
	,{"col1":"${BOM}val3","col2":"val4"}]
EOT
assertCmdEq "$CSV -aj < $SAMPLE/bom.csv" "$expectedOutput" \
	"csv's BOM stripping did not work as intended!"

# With the -M option, BOM stripping is disabled,
# so the BOM should now be read as part of the column name.
define expectedOutput <<-EOT
	[{"${BOM}col1":"val1","col2":"val2"}
	,{"${BOM}col1":"${BOM}val3","col2":"val4"}]
EOT
assertCmdEq "$CSV -ajM < $SAMPLE/bom.csv" "$expectedOutput" \
	"\"csv -M\" stripped the BOM from the header line!"


# If we skip the header line and go directly to the third line (which contains an extra BOM),
# it should be stripped.
define expectedOutput <<-EOT
	[["val3","val4"]]
EOT
assertCmdEq "$CSV -ij -s 2 < $SAMPLE/bom.csv" "$expectedOutput" \
	"csv's BOM stripping did not work when skipping to a line with a leading BOM!"

# Again, the -M option should preserve the BOM.
define expectedOutput <<-EOT
	[["${BOM}val3","val4"]]
EOT
assertCmdEq "$CSV -ijM -s 2 < $SAMPLE/bom.csv" "$expectedOutput" \
	"\"csv -M\" stripped the BOM from a skipped-to line!"


success
