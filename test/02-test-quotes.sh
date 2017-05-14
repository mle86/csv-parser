#!/bin/sh
. $(dirname "$0")/init.sh

fn1="col1"
fn2="long \"col name"
qv="backslash=\\\\ separator=; apos=' dq=\" eot"

define expectedOutput <<-EOT
	$fn1: first key
	$fn2: first value
	$RECORDSEP
	$fn1: second key
	$fn2: quote=" eot
	$RECORDSEP
	$fn1: third key
	$fn2: $qv
EOT

assertCmdEq "$CSV -am < $SAMPLE/quotes.csv" "$expectedOutput" \
	"csv did not handle \"-quoted values correctly!"


define expectedOutput <<-EOT
	$fn1: 1k
	$fn2: first value
	$RECORDSEP
	$fn1: 2k
	$fn2: $qv
	$RECORDSEP
	$fn1: 3k
	$fn2: $qv

EOT

assertCmdEq "$CSV -am < $SAMPLE/mixed-quotes.csv" "$expectedOutput" \
	"csv did not handle mixed-quoted values correctly!"

success
