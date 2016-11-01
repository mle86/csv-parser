#!/bin/bash
. $(dirname "$0")/init.sh

# If an input line ends with a separator character, it means there's a trailing empty field.
# There was a bug causing that empty field to be dropped from the very last input line
# if the input had no final linebreak.

# This is the expected behavior.
# There's two columns, the last field is empty.
assertCmdEq "printf 'k1;k2\n33;\n' | $CSV -j" '[{"k1":"33","k2":""}]'

# But does it still work if the input has no trailing linebreak?
# The output should be the same!
assertCmdEq "printf 'k1;k2\n33;'   | $CSV -j" '[{"k1":"33","k2":""}]'

# What if the last line has fewer fields than the header line?
# In this case, the output sohuld not name the second column, since there's no value.
# The trailing linebreak should make no difference.
assertCmdEq "printf 'k1;k2\n33\n' | $CSV -j" '[{"k1":"33"}]'
assertCmdEq "printf 'k1;k2\n33'   | $CSV -j" '[{"k1":"33"}]'

success
