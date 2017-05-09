#!/bin/sh
. $(dirname "$0")/init.sh

# There's a problem with column names
# of length 16 or 32 (and higher powers of two as well)
# in -X input mode -- extra characters are printed at the end.

input="1111111111222222,\na,"
expec="1111111111222222"

assertCmd "printf '$input' | $CSV -X"
col0name="$(printf '%s' "$ASSERTCMDOUTPUT" | grep "CSV_COLNAME_0=")"
assertEq "$col0name" "CSV_COLNAME_0=$expec" \
	"Column names of length 16 cause problems in -X input mode!"


input="33333333334444444444555555555566,\na,"
expec="33333333334444444444555555555566"

assertCmd "printf '$input' | $CSV -X"
col0name="$(printf '%s' "$ASSERTCMDOUTPUT" | grep "CSV_COLNAME_0=")"
assertEq "$col0name" "CSV_COLNAME_0=$expec" \
	"Column names of length 32 cause problems in -X input mode!"

success
