#!/bin/bash
. $(dirname "$0")/init.sh

# This bug was located in the output_fmt_escape_sequence() definition
# for the reformat_fn_t function (escape.c).
# It caused the last character of escapings build out of a format string (escape_other())
# to be dropped.


specialchars="printf 'A;B\\n\"0\\000\";\"1\\001\"\\nC;D'"


# Check if special characters in fields get correctly escaped:
define expectedOutput <<-EOT
	[{"A":"0\0","B":"1\u0001"}
	,{"A":"C","B":"D"}]
EOT
assertCmdEq "$specialchars | $CSV -abj" "$expectedOutput"

# Check if special characters in column names get correctly escaped:
define expectedOutput <<-EOT
	[{"0\0":"C","1\u0001":"D"}]
EOT
assertCmdEq "$specialchars | tail -n +2 | $CSV -abj" "$expectedOutput"


success
