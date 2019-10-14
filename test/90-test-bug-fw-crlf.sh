#!/bin/sh
. $(dirname "$0")/init.sh

# There was a bug in fw.c concerning the usage of count_lineend_len:
# If the calculated field length was 0 or 1
# but count_lineend_len() returned 2,
# it would result in an n value of (size_t)-1, i.e. an absurdly high number.
# This happens if a column boundary is just in the middle of an "\r\n" line end.

assertCmd "$CSV --fixed-width  A 1- B 9-  < $SAMPLE/fixed-crlf.fw"  # worked fine pre-fix
assertCmd "$CSV --fixed-width  A 1- B 10- < $SAMPLE/fixed-crlf.fw"  # caused segfault pre-fix

success
