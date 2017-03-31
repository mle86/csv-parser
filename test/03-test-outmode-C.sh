#!/bin/bash
. $(dirname "$0")/init.sh

define expectedOutput <<-EOT
	col1,col2
	first key,first value
	second key,"second

	value!
	"
	third key,third value
EOT


# assertCmdEq() will remove the final linebreak.
# We want the exact output, so we'll redirect it into a file
# and compare it manually.


cd_tmpdir
status=0

# convert our own regular LF linebreaks to CRLF:
printf '%s' "$expectedOutput" | perl -pe 's/\r?\n?$/\r\n/' > expected-output || status="$?"
add_cleanup expected-output

[ "$status" -eq 0 ] \
	|| skip "expected-output processing failed with status $status!"


$CSV -aCb < $SAMPLE/multiline.csv > csv-output || status="$?"
add_cleanup csv-output

[ "$status" -eq 0 ] \
	|| fail "csv -C failed with exit status $status!"
diff -q expected-output csv-output \
	|| fail "csv -C did not output multiline.csv's contents correctly!"


$CSV -aCb < $SAMPLE/multiline.csv | $CSV -aCb > csv-output2 || status="$?"
add_cleanup csv-output2

[ "$status" -eq 0 ] \
	|| fail "csv -C failed with exit status $status!"
diff -q expected-output csv-output2 \
	|| fail "csv could not parse its own -C output!"


success
