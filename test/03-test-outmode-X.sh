#!/bin/bash
. $(dirname "$0")/init.sh

assertCmd "$CSV -aXb < $SAMPLE/multiline.csv" 0 \
	"csv -X failed!"

errmsg_h="csv -X did not output multiline.csv's column names correctly!"
errmsg_c="csv -X did not output multiline.csv's contents correctly!"
errmsg_n="csv -X did not output multiline.csv's record count correctly!"
errmsg_hh="csv -X returned an extraneous column name!"
errmsg_cc="csv -X returned an extraneous cell value!"

cd_tmpdir
prepare_subshell <<-EOT
	# run the csv -X output, it should be shell var assignments:
	$ASSERTCMDOUTPUT

	# now check all those shell vars:

	assertEq "\$CSV_COLNAME_0" "$MLC1" "$errmsg_h"
	assertEq "\$CSV_COLNAME_1" "$MLC2" "$errmsg_h"

	assertEq "\$CSV_0_0" "$MLVA1" "$errmsg_c"
	assertEq "\$CSV_0_1" "$MLVA2" "$errmsg_c"
	assertEq "\$CSV_1_0" "$MLVB1" "$errmsg_c"
	assertEq "\$CSV_1_1" "$MLXB2" "$errmsg_c"
	assertEq "\$CSV_2_0" "$MLVC1" "$errmsg_c"
	assertEq "\$CSV_2_1" "$MLVC2" "$errmsg_c"

	assertEmpty "\$CSV_COLNAME_2" "$errmsg_hh"
	assertEmpty "\$CSV_0_2" "$errmsg_cc"
	assertEmpty "\$CSV_2_2" "$errmsg_cc"
	assertEmpty "\$CSV_3_0" "$errmsg_cc"
	
	assertEq "\$CSV_RECORDS" "3"
EOT
$TMPSH
assertSubshellWasExecuted

success
