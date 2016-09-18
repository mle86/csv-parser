#!/bin/bash
. $(dirname "$0")/init.sh

# Tests whether -b correctly reads multiline cells:

define expectedOutput <<-EOT
	$MLC1: $MLVA1
	$MLC2: $MLVA2
	$RECORDSEP
	$MLC1: $MLVB1
	$MLC2: $MLVB2
	$RECORDSEP
	$MLC1: $MLVC1
	$MLC2: $MLVC2
EOT

assertCmdEq "$CSV -ab < $SAMPLE/multiline.csv" "$expectedOutput" \
	"csv -b did not read multiline.csv correctly!"

success
