#!/bin/bash
. $(dirname "$0")/init.sh

# Try with all columns:

fn1="fn1"
fn2="fn2"
fn3="fn3"
fn4="fn4"
fn5="fn5"
collist="$fn1 $fn2 $fn3 $fn4 $fn5"
define expectedOutput <<-EOT
	$fn1: l1c1
	$fn2: l1c2
	$fn3: l1c3
	$fn4: l1c4
	$fn5: l1c5
	$RECORDSEP
	$fn1: l2c1
	$fn2: l2c2
	$fn3: l2c3
	$fn4: l2c4
	$fn5: l2c5
	$RECORDSEP
	$fn1: l3c1
	$fn2: l3c2
	$fn3: l3c3
	$fn4: l3c4
	$fn5: l3c5
EOT

assertCmdEq "$CSV -g $collist < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -g did not read sample1.csv correctly!"


# Try with short fieldname list:

fn1="yyfn1"
fn2="yyfn2"
collist="$fn1 $fn2"
define expectedOutput <<-EOT
	$fn1: l1c1
	$fn2: l1c2
	$RECORDSEP
	$fn1: l2c1
	$fn2: l2c2
	$RECORDSEP
	$fn1: l3c1
	$fn2: l3c2
EOT
assertCmdEq "$CSV -g $collist < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -g did not work correctly when given less fieldname arguments than csv columns! Should have ignored the extra csv columns."


# Try with ignored field:

fn1="iifn1"
fn2="iifn2"
# skip first and third column,
# ignore fifth column:
collist="@ $fn1 @ $fn2"
define expectedOutput <<-EOT
	$fn1: l1c2
	$fn2: l1c4
	$RECORDSEP
	$fn1: l2c2
	$fn2: l2c4
	$RECORDSEP
	$fn1: l3c2
	$fn2: l3c4
EOT
assertCmdEq "$CSV -g $collist < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -g did not work correctly when given '@' arguments! Should have skipped the @ columns."

success
