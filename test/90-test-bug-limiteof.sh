#!/bin/sh
. $(dirname "$0")/init.sh

# If the --skip option skips all (remaining) records, there should be no error --
# the program should simply exit with empty output.
# (This has always worked fine without the -b option,
#  but since this version's changes to skip(),
#  it throws an error with the -b option.)

SAMPLE="$HERE/samples/sample1.csv"


# Usual case: We skip some lines but there are remaining records in the input.

define expectedOutput <<-EOT
	[["l3c1","l3c2","l3c3","l3c4","l3c5"]]
EOT

assertCmdEq "$CSV -ji  -s 2 <$SAMPLE" "$expectedOutput"
assertCmdEq "$CSV -jib -s 2 <$SAMPLE" "$expectedOutput"


# Now we exactly skip all input records.

define expectedOutput <<-EOT
	[]
EOT

assertCmdEq "$CSV -ji  -s 3 <$SAMPLE" "$expectedOutput"
assertCmdEq "$CSV -jib -s 3 <$SAMPLE" "$expectedOutput"


# Now we skip all input records by a considerable margin.

define expectedOutput <<-EOT
	[]
EOT

assertCmdEq "$CSV -ji  -s 999 <$SAMPLE" "$expectedOutput"
assertCmdEq "$CSV -jib -s 999 <$SAMPLE" "$expectedOutput"


success
