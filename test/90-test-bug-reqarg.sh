#!/bin/sh
. $(dirname "$0")/init.sh

# The -d option was marked as "has an argument",
# but the --separator option was not.
# Using the --separator option (with or without an argument)
# caused the program to segfault.

assertCmd   "$CSV -i --separator </dev/null" $EXIT_SYNTAX
assertCmdEq "$CSV -i --separator ',' </dev/null" ""

success
