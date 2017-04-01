#!/bin/sh
. $(dirname "$0")/init.sh

# Verify that the program can do colored output,
# does not colorize redirected output by default,
# and that "--color never" works.

define expectedOutput <<-EOT
	[{"l1c1":"l2c1","l1c2":"l2c2","l1c3":"l2c3","l1c4":"l2c4","l1c5":"l2c5"}
	,{"l1c1":"l3c1","l1c2":"l3c2","l1c3":"l3c3","l1c4":"l3c4","l1c5":"l3c5"}]
EOT

# All the earlier tests would have failed if there had been any colors!
# Still, test it explicitly:
assertCmdEq "$CSV -aj < $SAMPLE/sample1.csv" "$expectedOutput" \
	"unexpected output!"
# The same should happen with an explicit "--color=never":
assertCmdEq "$CSV -aj --color=never < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv --color=never produced unexpected output, whereas it worked properly without the --color option!"
# The "--color=auto" setting should recognize this as a redirection, and omit colors:
assertCmdEq "$CSV -aj --color=auto < $SAMPLE/sample1.csv" "$expectedOutput" \
	"csv --color=auto produced unexpected output, whereas it worked properly without the --color option and with --color=never!"

# Using "--color" without a WHEN setting could mean "always" or "auto".
# This test does not care, we only care that it works and does not eat following options:
assertCmd "$CSV --color -aj < $SAMPLE/sample1.csv" 0 \
	"csv --color (without a WHEN setting) exited with an unexpected exit code while reading sample1.csv!"

assertCmd "$CSV -aj --color=always < $SAMPLE/sample1.csv" 0 \
	"csv --color=always exited with an unexpected exit code while reading sample1.csv!"

[ "$ASSERTCMDOUTPUT" != "$expectedOutput" ] || \
	fail "csv --color=always produced the same output as csv --color=never!"

case "$ASSERTCMDOUTPUT" in
	*"["*"m"*)  ;;
	*)
		err "csv --color=always seems to have produced output without any ANSI escape sequences!"
		err "This is the full output:"
		fail "$ASSERTCMDOUTPUT"
		;;
esac


success
