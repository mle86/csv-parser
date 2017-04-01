#!/bin/sh
. $(dirname "$0")/init.sh

# This bug only appears in -n input mode
# and only if the names lists have no aliases (cd.names == 1),
# because the max_names values only got increased when reading aliases,
# not increased to 1 when reading a basename.

assertCmdEq "echo 'Aaa;Bbb;Ccc' | $CSV -nj Uuu . Bbb . Xxx ." "[]"

# Just a header line, but with one column match:
# this should result in plain empty output.
# If the bug re-appears, the program won't match any columns,
# and die with EXIT_FORMAT ("could not match any column names").

success
