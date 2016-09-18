#!/bin/bash
. $(dirname "$0")/init.sh

# In -a mode, a header line is expected;
# a line limit of -l 1 should cause the program to read the header line and 1 line after that.

define expectedOutput <<-EOT
	l1c1: l2c1
	l1c2: l2c2
	l1c3: l2c3
	l1c4: l2c4
	l1c5: l2c5
EOT

assertCmdEq "$CSV -a -l1 < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -al1 failed!"

# Skipping one line (-s 1) should NOT skip the header line, it should only skip the first "content" line.
# Still, the one-line limit should apply:

define expectedOutput <<-EOT
	l1c1: l3c1
	l1c2: l3c2
	l1c3: l3c3
	l1c4: l3c4
	l1c5: l3c5
EOT

assertCmdEq "$CSV -a -s1 -l1 < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -as1l1 failed!"


# In -i mode, no header line is expected. Input processing should now stop after the first input line.

define expectedOutput <<-EOT
	0: l1c1
	1: l1c2
	2: l1c3
	3: l1c4
	4: l1c5
EOT

assertCmdEq "$CSV -i -l1 < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -il1 failed!"

# Again, but skip the first line:

define expectedOutput <<-EOT
	0: l2c1
	1: l2c2
	2: l2c3
	3: l2c4
	4: l2c5
EOT

assertCmdEq "$CSV -i -s1 -l1 < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -is1l1 failed!"


success
