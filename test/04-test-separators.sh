#!/bin/bash
. $(dirname "$0")/init.sh

# All tests so far have assumed the semicolon separator.
# Let's try auto-detection of a different separator.
# separators.csv's first separator-like character is a pipe:

assertCmd "$CSV -aim < $SAMPLE/separators.csv" 0 \
	"csv failed while reading separators.csv!"

assertContains "$ASSERTCMDOUTPUT" "0: hdr1" \
	"csv did not correctly recognize a special separator character in the header line!"
assertContains "$ASSERTCMDOUTPUT" "1: hdr2;a" \
	"csv incorrectly recognized a second separator character in the header line!"

errmsg="csv did not correctly split lines on a special separator character!"
assertContains "$ASSERTCMDOUTPUT" "0: cell3" "$errmsg"
assertContains "$ASSERTCMDOUTPUT" "1: a.b${TAB}c;d,cell4" "$errmsg"

# Now let's try a different, manually-set separator character
# which is not contained in the header line:

assertCmd "$CSV -aim -d, < $SAMPLE/separators.csv" 0 \
	"csv -d',' failed while reading separators.csv!"
errmsg="csv -d did not correctly split lines on a manually-set separator character!"
assertContains "$ASSERTCMDOUTPUT" "0: hdr1|hdr2;a" "$errmsg"
assertContains "$ASSERTCMDOUTPUT" "0: cell3|a.b${TAB}c;d" "$errmsg"
assertContains "$ASSERTCMDOUTPUT" "1: cell4" "$errmsg"

success
