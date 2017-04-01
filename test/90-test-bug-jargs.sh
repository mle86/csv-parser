#!/bin/sh
. $(dirname "$0")/init.sh

# The -j option (long json output) was falsely assigned the long option --compact-json,
# and the -J option (compact output) was falsely assigned the long option --json.

assertCmd   "$CSV -i -J             </dev/null" $EXIT_SYNTAX
assertCmd   "$CSV -i --compact-json </dev/null" $EXIT_SYNTAX

assertCmdEq "printf 'Aa;Bb\n' | $CSV -i -j"     "[[\"Aa\",\"Bb\"]]"
assertCmdEq "printf 'Aa;Bb\n' | $CSV -i --json" "[[\"Aa\",\"Bb\"]]"

success
