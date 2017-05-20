#!/bin/sh
. $(dirname "$0")/init.sh

# This test ensures that the program's error messages
# contain the correct input line number,
# with or without --skip and/or --limit options.


SAMPLE="$HERE/samples/malformed2.csv"
# This 7-line file has formatting errors on lines 2 and 6 when read without the -b option.
# (In -b mode, line 2 is valid and the start of a multi-line record,
#  but line 6 still contains a formatting error.)

test_lineno () {
	local expected_lineno="$1" ; shift
	if [ "$expected_lineno" -eq 0 ]; then
		# should succeed
		assertCmd "$CSV -ij $* 2>&1 >/dev/null <$SAMPLE" 0
	else
		# should fail and mention the lineno in the output
		assertCmd "$CSV -ij $* 2>&1 >/dev/null <$SAMPLE" "$EXIT_FORMAT"
		assertContains "$ASSERTCMDOUTPUT" "unexpected end of line on line $expected_lineno" \
			"csv with options '$*' did not fail with the correct line number!"
	fi
	true
}


test_lineno 2
test_lineno 2  --skip 1
test_lineno 2  --skip 1 --limit 1

test_lineno 6  --skip 2
test_lineno  0 --skip 2 --limit 1
test_lineno 6  --skip 5
test_lineno 6  --skip 5 --limit 1
test_lineno 6  --skip 5 --limit 2


success
