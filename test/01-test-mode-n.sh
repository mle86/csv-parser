#!/bin/bash
. $(dirname "$0")/init.sh

target_fn1="COLN1" ; aliases_fn1="l1c1 foo-bar"
target_fn2="l1c2"  ; aliases_fn2=""  # no aliases, target name is the actual column name
target_fn3="COLN3" ; aliases_fn3="rhubarb l1c3 l1c6"  # l1c3 is found first, so this alias list should not claim l1c6
target_fn5="COLN5" ; aliases_fn5="lemon"  # not found at all
# skip l1c4 and l1c5
collist=\
"$target_fn1 $aliases_fn1 . "\
"$target_fn2 $aliases_fn2 . "\
"$target_fn3 $aliases_fn3 . "\
"$target_fn5 $aliases_fn5 . "

define expectedOutput <<-EOT
	$target_fn1: l2c1
	$target_fn2: l2c2
	$target_fn3: l2c3
	$RECORDSEP
	$target_fn1: l3c1
	$target_fn2: l3c2
	$target_fn3: l3c3
EOT

assertCmdEq "$CSV -n $collist < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -n did not read sample1.csv correctly!"

success
