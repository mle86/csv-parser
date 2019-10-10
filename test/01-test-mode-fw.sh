#!/bin/sh
. $(dirname "$0")/init.sh


#hdr1hdr2hdr3
#V000V222V444
#W111W222W333
coldef='XA 1-4  XB 5-8  XC 9-12'


# This is the basic case.
# No surprises here, all columns are the same length.

define expectedOutput <<-EOT
	XA: hdr1
	XB: hdr2
	XC: hdr3
	$RECORDSEP
	XA: V000
	XB: V222
	XC: V444
	$RECORDSEP
	XA: W111
	XB: W222
	XC: W333
EOT

assertCmdEq "$CSV -m --fixed-width $coldef < $SAMPLE/fixed1.fw" "$expectedOutput" \
	"csv --fixed-width did not read fixed1.fw correctly!"

success
