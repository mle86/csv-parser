#!/bin/sh
. $(dirname "$0")/init.sh

# The --fixed-width input mode has already been tested in 01-test-mode-fw,
# but only its most basic functionality
# and without any interesting option combinations.

# This test depends on test-mode-fw, test-filter, and test-trim.


# Line 3 of fixed2.fw is shorter than the rest.
# With a “COLNAME 9-” definition that's not a problem
# and neither is “COLNAME 9-10”,
# but “COLNAME 9-12” should fail.

define expectedOutput <<-EOT
	XA: T777
	XC: T9
EOT

assertCmd   "$CSV --limit 1 --skip 2 --fixed-width  XA 1-4  XC 9-12  < $SAMPLE/fixed2.fw" "$EXIT_FORMAT"

assertCmdEq "$CSV --limit 1 --skip 2 --fixed-width  XA 1-4  XC 9-    < $SAMPLE/fixed2.fw" "$expectedOutput" \
	"csv --fixed-width COL POS- did not work as expected!"


# Test field trimming.
# Line 1 of fixed2.fw is left-padded,
# line 2 is right-padded,
# and line 3 is right-padded and shorter than the rest.

#L111 L22  L3
#R444R55 R6  
#T777T88 T9
define expectedOutput <<-EOT
	X1: L111
	X2: L22
	X3: L3
	-
	X1: R444
	X2: R55
	X3: R6
	-
	X1: T777
	X2: T88
	X3: T9
EOT

assertCmdEq "$CSV --limit 3 --trim=fields --fixed-width  X1 1-4  X2 5-8  X3 9-  < $SAMPLE/fixed2.fw" "$expectedOutput" \
	"csv --fixed-width --trim=fields did not work as expected!"


# Test filtering.
# Line 4 of fixed2.fw contains spaces only!

define expectedUnfilteredOutput <<-EOT
	Y1:     
	Y2:     
	Y3:     
EOT

define expectedFilteredOutput <<-EOT
EOT

assertCmdEq "$CSV --limit 1 --skip 3 --filter=none --fixed-width  Y1 1-4  Y2 5-8  Y3 9-  < $SAMPLE/fixed2.fw" "$expectedUnfilteredOutput" \
	"csv --fixed-width --filter=none did not work as expected!"
assertCmdEq "$CSV --limit 1 --skip 3 --filter=blank --fixed-width  Y1 1-4  Y2 5-8  Y3 9-  < $SAMPLE/fixed2.fw" "$expectedFilteredOutput" \
	"csv --fixed-width --filter=blank did not work as expected on an all-spaces input line!"


# Test overlapping and repeated column definitions.

#V000V222V444
define expectedOverlapOutput <<-EOT
	C1: V000
	C1: V000
	C23: V222V444
	C2: V222
	C3: V444
EOT

assertCmd "$CSV --limit 1 --skip 1 --fixed-width  C1 1-4  C2 5-8  C3 9-12  C1 1-4  C23 5-12  < $SAMPLE/fixed1.fw"

# csv makes no guarantee about the column output, so we'd better sort it in this case:
ASSERTCMDOUTPUT="$(printf '%s\n' "$ASSERTCMDOUTPUT" | LC_ALL=C sort)"
assertEq "$ASSERTCMDOUTPUT" "$expectedOverlapOutput" \
	"csv --fixed-width did not work as expected on overlapping/repeated column definitions!"


# Test automatic end position setting.

#V000V222V444
define expectedAutoEndOutput <<-EOT
	A1: V000
	A2: V222
	A3: V444
EOT

assertCmdEq "$CSV --limit 1 --skip 1 --fixed-width  A1 1  A2 5  A3 9  < $SAMPLE/fixed1.fw" "$expectedAutoEndOutput" \
	"csv --fixed-width did not work as expected when omitting all end positions!"


success
