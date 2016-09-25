#!/bin/bash
. $(dirname "$0")/init.sh

# Auto-numbered columns:
fn1="0"
fn2="1"
fn3="2"
fn4="3"
fn5="4"

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

assertCmdEq "$CSV -im < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv -a did not read sample1.csv correctly!"

success
