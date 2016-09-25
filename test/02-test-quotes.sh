#!/bin/bash
. $(dirname "$0")/init.sh

fn1="col1"
fn2="long \"col name"

define expectedOutput <<-EOT
	$fn1: first key
	$fn2: first value
	$RECORDSEP
	$fn1: second key
	$fn2: quote=" eot
	$RECORDSEP
	$fn1: third key
	$fn2: backslash=\\\\ separator=; eot
EOT

assertCmdEq "$CSV -am < $SAMPLE/quotes.csv" "$expectedOutput" \
	"csv did not handle \"-quoted values correctly!"

success
