#!/bin/sh
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

assertCmdEq "$CSV -am -l1 < $SAMPLE/sample1.csv" "$expectedOutput" \
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

assertCmdEq "$CSV -am -s1 -l1 < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -as1l1 failed!"


# In -i mode, no header line is expected. Input processing should now stop after the first input line.

define expectedOutput <<-EOT
	0: l1c1
	1: l1c2
	2: l1c3
	3: l1c4
	4: l1c5
EOT

assertCmdEq "$CSV -im -l1 < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -il1 failed!"

# Again, but skip the first line:

define expectedOutput <<-EOT
	0: l2c1
	1: l2c2
	2: l2c3
	3: l2c4
	4: l2c5
EOT

assertCmdEq "$CSV -im -s1 -l1 < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -is1l1 failed!"

# The --fixed-width mode does not treat the first line specially (similar to -g),
# but internally its record counting process is a bit different.
# Make sure it still works as expected:

define expectedOutput <<-EOT
	X1: V0
	X2: V2
	X3: V4
EOT

assertCmdEq "$CSV -m -s1 -l1 --fixed-width  X1 1-2  X2 5-6  X3 9-10 < $SAMPLE/fixed1.fw" "$expectedOutput" \
	"csv --fixed-width -s1l1 failed!"


success
