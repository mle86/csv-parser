#!/bin/sh
. $(dirname "$0")/init.sh

# If an input line ends with a separator character, it means there's a trailing empty field.
# There was a bug causing that empty field to be dropped from the very last input line
# if the input had no final linebreak.

SAMPLE="$HERE/samples/multiline2.csv"


# This is the expected behaviour without --limit and --skip:
# Multi-line fields are processed correctly.
define expectedOutput <<-EOT
	[["A1","A2\\nax"]
	,["B1","B2\\nbx"]
	,["C1","C2\\ncx"]
	,["D1","D2\\ndx"]]
EOT

assertCmdEq "$CSV -jib <$SAMPLE" "$expectedOutput"


# Skipping the first record (-s 1) should result in this output:
define expectedOutput <<-EOT
	[["B1","B2\\nbx"]
	,["C1","C2\\ncx"]
	,["D1","D2\\ndx"]]
EOT

assertCmdEq "$CSV -jib --skip 1 <$SAMPLE || true" "$expectedOutput" \
	"csv -b --skip=1 did not work correctly on a multi-line record!"


# Limiting the number of records (-l 1) should result in this output:
define expectedOutput <<-EOT
	[["A1","A2\\nax"]]
EOT

assertCmdEq "$CSV -jib --limit 1 <$SAMPLE || true" "$expectedOutput" \
	"csv -b --limit=1 did not work correctly on a multi-line record!"


# Now try a combination of --limit and --skip:
define expectedOutput <<-EOT
	[["B1","B2\\nbx"]]
EOT

assertCmdEq "$CSV -jib --limit 1 --skip 1 <$SAMPLE || true" "$expectedOutput" \
	"csv -b --limit=1 --skip=1 did not work correctly on a multi-line record!"


# So far we've just tested regular multi-lined columns.
# What about multi-lined column names?
# Will --skip and --limit still work correctly?

define expectedOutput <<-EOT
	[{"A1":"C1","A2\nax":"C2\ncx"}]
EOT

assertCmdEq "$CSV -jab --limit 1 --skip 1 <$SAMPLE || true" "$expectedOutput" \
	"csv -b --limit=1 --skip=1 did not work correctly on a multi-line record with multi-line column names!"


success
