#!/bin/bash
. $(dirname "$0")/init.sh

# Some modes require input (-n, -a),
# because they need to scan the column names from the header line.
# Running them with empty input should result in an error status.

emptyFile='/dev/null'

assertCmd "$CSV -nj <$emptyFile" "$EXIT_EMPTY" \
	"csv -n did NOT fail on empty input!"
assertEmpty "$ASSERTCMDOUTPUT" \
	"csv -nj failed on empty input, but still produced stdout output!"

assertCmd "$CSV -aj <$emptyFile" "$EXIT_EMPTY" \
	"csv -a did NOT fail on empty input!"
assertEmpty "$ASSERTCMDOUTPUT" \
	"csv -aj failed on empty input, but still produced stdout output!"


# Other modes (-g, -i) don't treat the first line specially,
# so they should be able to work with empty input
# and should produce appropriate output.

match_regex () {
	local regex="$1"
	perl -e "local \$/; exit <> !~ m${regex}" || return
}

check_empty_json () {
	local used_args="$1"
	if ! echo "$ASSERTCMDOUTPUT" | match_regex '/^\s*\[\s*\]\s*$/'; then
		err  "csv $used_args produced no valid json on empty input!"
		fail "($ASSERTCMDOUTPUT)"
	fi
}

check_empty_compact () {
	local used_args="$1"
	if ! echo "$ASSERTCMDOUTPUT" | match_regex '/^\s*\{(?=[^}]*?"columns":\s*\[\s*\])(?=[^}]*?"lines":\s*\[\s*\]).+?\}\s*$/s'; then
		err  "csv $used_args produced no valid compact json on empty input!"
		fail "($ASSERTCMDOUTPUT)"
	fi
}

check_empty_shellvars () {
	local used_args="$1"
	prepare_subshell <<-EOT
		$ASSERTCMDOUTPUT  # exec var assignments
		assertEmpty "\$CSV_COLNAME_0"   "csv $used_args produced unexpected output on empty input!"
		assertEmpty "\$CSV_0_0"         "csv $used_args produced unexpected output on empty input!"
		assertEq    "\$CSV_RECORDS" "0" "csv $used_args produced incorrect output on empty input!"
	EOT
	$TMPSH
	assertSubshellWasExecuted
}


assertCmd "$CSV -gj Col1 Col2 <$emptyFile" 0 \
	"csv -g failed on empty input!"
check_empty_json '-gj'

assertCmd "$CSV -ij <$emptyFile" 0 \
	"csv -i failed on empty input!"
check_empty_json '-ij'

assertCmd "$CSV -gJ Col1 Col2 <$emptyFile" 0
check_empty_compact '-gJ'

cd_tmpdir

assertCmd "$CSV -gX Col1 Col2 <$emptyFile" 0
check_empty_shellvars '-gX'

assertCmd "$CSV -iX <$emptyFile" 0
check_empty_shellvars '-iX'


success
