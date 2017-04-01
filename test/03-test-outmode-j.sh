#!/bin/sh
. $(dirname "$0")/init.sh

define expectedOutput <<-EOT
	[{"$MLC1":"$MLVA1","$MLC2":"$MLVA2"}
	,{"$MLC1":"$MLVB1","$MLC2":"$MLVB2"}
	,{"$MLC1":"$MLVC1","$MLC2":"$MLVC2"}]
EOT

assertCmdEq "$CSV -ajb < $SAMPLE/multiline.csv" "$expectedOutput" \
	"csv -j did not output multiline.csv's contents correctly!"

success
