#!/bin/bash
. $(dirname "$0")/init.sh

# The -n mode by default discards all unknown columns.
# This behavior is already checked by test-mode-n.
# With the -u (--unknowns) option however,
# the -n input mode should include all unknown column by their original name as well.

# sample1 contains the columns "l1c[1-5]".

target_fn1='COL1' ; alias_fn1='X1c1 l1c1'
target_fn3='COL3' ; alias_fn3='l1c3 X1c3'
collist=\
"$target_fn3 $alias_fn3 . "\
"$target_fn1 $alias_fn1 . "

# Since we only specified COL1 (l1c1) and COL2 (l1c3) manually,
# we should expect -nu to include l1c2,l1c4,l1c5 by their original names:

define expectedOutput <<-EOT
	$target_fn1: l2c1
	l1c2: l2c2
	$target_fn3: l2c3
	l1c4: l2c4
	l1c5: l2c5
	$RECORDSEP
	$target_fn1: l3c1
	l1c2: l3c2
	$target_fn3: l3c3
	l1c4: l3c4
	l1c5: l3c5
EOT

assertCmdEq "$CSV -num $collist < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -n -u did not correctly include all unknown columns!"


success
