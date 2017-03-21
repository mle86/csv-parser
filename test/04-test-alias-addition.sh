#!/bin/bash
. $(dirname "$0")/init.sh

# The -n mode should allow multiple alias lists for the same column name. That is,
# -n COL1 alias1 . COL1 alias2 .
# should have the same effect as
# -n COL1 alias1 alias2 .

target_fn1="COLN1" ; aliases_fn1="xxx1" ; addition_fn1="l1c1"
target_fn2="COLN2" ; aliases_fn2="l1c2" ; addition_fn2="xxx2"
collist=\
"$target_fn1 $aliases_fn1 . "\
"$target_fn2 $aliases_fn2 . "\
"$target_fn1 $addition_fn1 . "\
"$target_fn2 $addition_fn2 . "\

define expectedOutput <<-EOT
	$target_fn1: l2c1
	$target_fn2: l2c2
	$RECORDSEP
	$target_fn1: l3c1
	$target_fn2: l3c2
EOT

assertCmdEq "$CSV -nm $collist < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -n did not correctly process additional column aliases!"


success
