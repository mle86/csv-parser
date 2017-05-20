#!/bin/sh
. $(dirname "$0")/init.sh

# All tests so far had input with simple LF (\n) line ends.
# However, RFC 4180 stipulates CRLF line ends.
# Our program should accept both styles, with identical results.

define expectedOutput <<-EOT
	[["c1","c2"]
	,["v1","v2"]]
EOT

testeol () {
	local errmsg="$1"
	assertCmdEq "printf 'c1;c2${rs}v1;v2${rs}' | $CSV -ij" "$expectedOutput" "$errmsg"
}

rs="\\n"  # business as usual
testeol "csv produced unexpected output!"

rs="\\r\\n"  # CRLF, as in RFC 4180
testeol "csv did not handle CRLF line ends correctly!"

success
