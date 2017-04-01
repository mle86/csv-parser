#!/bin/sh
. $(dirname "$0")/init.sh

define expectedOutput <<-EOT
{"columns": ["$MLC1","$MLC2"],
"lines": [
 ["$MLVA1","$MLVA2"]
,["$MLVB1","$MLVB2"]
,["$MLVC1","$MLVC2"]]}
EOT

assertCmdEq "$CSV -aJb < $SAMPLE/multiline.csv" "$expectedOutput" \
	"csv -J did not output multiline.csv's contents correctly!"

success
