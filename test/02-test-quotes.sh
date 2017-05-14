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

# Doublequoted input should always be recognized correctly, even without any -q option.
assertCmdEq "$CSV -am < $SAMPLE/quotes.csv" "$expectedOutput" \
	"csv did not handle \"-quoted values correctly!"

# The input contains only doublequotes, so an explicit -q" should not change anything.
assertCmdEq "$CSV -am -q\\\" < $SAMPLE/quotes.csv" "$expectedOutput" \
	"csv -q'\"' did not handle \"-quoted values correctly!"

# The "-q auto" option should work as well.
assertCmdEq "$CSV -am -q auto < $SAMPLE/quotes.csv" "$expectedOutput" \
	"csv --quotes=auto did not handle \"-quoted values correctly!"

# The "-q mixed" setting should work as well.
assertCmdEq "$CSV -am -q mixed < $SAMPLE/quotes.csv" "$expectedOutput" \
	"csv --quotes=mixed did not handle \"-quoted values correctly!"


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

assertCmdEq "$CSV -am -q mixed < $SAMPLE/mixed-quotes.csv" "$expectedOutput" \
	"csv --quotes=mixed did not handle mixed-quoted values correctly!"

# mixed mode is default!
assertCmdEq "$CSV -am < $SAMPLE/mixed-quotes.csv" "$expectedOutput" \
	"csv (without -q option) did not handle mixed-quoted values correctly!"


# samples/quotes2.csv contains a mixed-quote style as well.
# It contains no "+" characters, so we can use that to test manual quotes setting.

define expectedOutput <<-EOT
	x: "This is a ""-quoted value."
	-
	x: 'This looks like a ''-quoted value.'
EOT

assertCmdEq "$CSV -am -q+ < $SAMPLE/quotes2.csv" "$expectedOutput" \
	"csv -q'+' did not handle mixed-quoted values correctly!"

# Now let's see what -q auto makes of this file.
# Since the "-quoted field comes first, the '-quoted field should be treated as unquoted.

define expectedOutput <<-EOT
	x: This is a "-quoted value.
	-
	x: 'This looks like a ''-quoted value.'
EOT

assertCmdEq "$CSV -am -q auto < $SAMPLE/quotes2.csv" "$expectedOutput" \
	"csv --quotes=auto did not handle mixed-quoted values correctly!"

# Lastly, let's see if -q' works as expected:

define expectedOutput <<-EOT
	x: "This is a ""-quoted value."
	-
	x: This looks like a '-quoted value.
EOT

assertCmdEq "$CSV -am -q\\' < $SAMPLE/quotes2.csv" "$expectedOutput" \
	"csv -q\"'\" did not handle mixed-quoted values correctly!"


success
