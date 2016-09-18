#!/bin/sh

SAMPLE="$HERE/samples"
CSV="$(readlink -f -- "$HERE/../csv")"

RECORDSEP="-"

TAB="	"

# copied from const.h:
EXIT_HELP=0
EXIT_SYNTAX=1
EXIT_FORMAT=2
EXIT_NOSEP=3
EXIT_EMPTY=4
EXIT_INTERNAL=10

# samples/multiline.csv:
MLC1='col1'
MLC2='col2'
MLVA1="first key"
MLVA2="first value"
MLVB1="second key"
MLVB2="second\\n\\nvalue!\\n"
MLXB2="second

value!
"
MLVC1="third key"
MLVC2="third value"


# define [-t] varname < input
#  Assigns its stdin input to a variable.
#  Similar to the `read' builtin, but it does not care about newlines.
#  This is useful to assign a heredoc to a variable.
#  If trailing newlines should be preserved, use the -t option.
#  Without that option, _all_ trailing newline characters will be removed,
#  similar to $(...) assignments.
#  Note that the -t option will still remove _one_ trailing newline character (if present).
#  Returns true if stdin was readable, false otherwise (in this case, $varname is not changed).
define () {
	local preserve_newlines=
	if [ "$1" = "-t" ]; then
		preserve_newlines=yes
		shift
	fi
	local varname="$1"
	local br="
"

	if [ "$preserve_newlines" ]; then
		output="$(cat 2>/dev/null ; echo -n "EOT")" || return
		output="${output%"EOT"}"
		output="${output%"$br"}"
	else
		output="$(cat 2>/dev/null)" || return
	fi
	eval "$varname=\"\$output\""
}

