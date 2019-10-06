#!/bin/sh
. $(dirname "$0")/init.sh

# The --filter option can be used to remove all empty records from the input file,
# i.e. records consisting only of empty fields, "0" fields, or whitespace fields,
# depending on the filter settings.

# This test depends on test-limitskip and test-whitespace.

# Make sure that --filter=none works the same as no --filter option at all,
# regardless of the input file:
assertCmd "$CSV -ij < $SAMPLE/sample1.csv"
assertCmdEq "$CSV -ij --filter=none < $SAMPLE/sample1.csv" "$ASSERTCMDOUTPUT" \
	"csv --filter=none produced incorrect output reading sample1.csv!"

assertCmd "$CSV -ij < $SAMPLE/whitespace.csv"
assertCmdEq "$CSV -ij --filter=none < $SAMPLE/whitespace.csv" "$ASSERTCMDOUTPUT" \
	"csv --filter=none produced incorrect output reading whitespace.csv!"

assertCmd "$CSV -ij < $SAMPLE/filter1.csv"
FILTER1_OUTPUT_UNFILTERED="$ASSERTCMDOUTPUT"
assertCmdEq "$CSV -ij --filter=none < $SAMPLE/filter1.csv" "$ASSERTCMDOUTPUT" \
	"csv --filter=none produced incorrect output reading filter1.csv!"


define expectedEmptyFilterOutput <<-EOT
	[["h1","h2"]
	,["","v1"]
	,["","v2"]]
EOT

define expectedEmptyZeroFilterOutput <<-EOT
	[["h1","h2"]
	,["0","v1"]
	,["0","v2"]]
EOT

# With --filter=empty, csv should drop all records that consist of empty strings only.
# It should NOT remove records that happen to contain one empty field (but are not completely empty).
assertCmdEq "$CSV -ij --filter=empty < $SAMPLE/filter1.csv" "$expectedEmptyFilterOutput" \
	"csv --filter=empty produced incorrect output reading filter1.csv!"

# filter1.csv contains no zeroes, so --filter=zero should have absolutely no effect.
assertCmdEq "$CSV -ij --filter=zero < $SAMPLE/filter1.csv" "$FILTER1_OUTPUT_UNFILTERED" \
	"csv --filter=zero had an effect on filter1.csv!"

# --filter=empty-or-zero should remove most lines from filter2.csv.
assertCmdEq "$CSV -ij --filter=empty-or-zero < $SAMPLE/filter2.csv" "$expectedEmptyZeroFilterOutput" \
	"csv --filter=empty-or-zero produced incorrect output reading filter2.csv!"

# whitespace.csv contains nothing but various whitespace in two columns.
# With --filter=blank, the result should be zero records.
assertCmdEq "$CSV -ij --filter=blank < $SAMPLE/whitespace.csv" "[]" \
	"csv --filter=blank produced non-empty output from whitespace.csv!"


# The --skip=N option is not affected by --filter,
# it still skips the first N records in the file regardless of their content.
# The first two records of filter3.csv are empty, but --skip should not care about that:

define expectedSkipUnfilteredOutput <<-EOT
	[{"h1":"aa","h2":"bb"}
	,{"h1":"","h2":""}
	,{"h1":"cc","h2":"dd"}]
EOT

define expectedSkipFilteredOutput <<-EOT
	[{"h1":"aa","h2":"bb"}
	,{"h1":"cc","h2":"dd"}]
EOT

assertCmdEq "$CSV -j --skip=2 --filter=none  < $SAMPLE/filter3.csv" "$expectedSkipUnfilteredOutput" \
	"--skip produced unexpected output even when using --filter=none!"
assertCmdEq "$CSV -j --skip=2 --filter=empty < $SAMPLE/filter3.csv" "$expectedSkipFilteredOutput" \
	"--skip produced unexpected output while using --filter!"


# The --limit=N option limits the total number of output records.
# This means that filtered records should not count towards that limit.

define expectedLimit1UnfilteredOutput <<-EOT
	[{"h1":"","h2":""}]
EOT
define expectedLimit1FilteredOutput <<-EOT
	[{"h1":"aa","h2":"bb"}]
EOT

define expectedLimit2FilteredOutput <<-EOT
	[{"h1":"aa","h2":"bb"}
	,{"h1":"cc","h2":"dd"}]
EOT

assertCmdEq "$CSV -j --limit=1 --filter=none < $SAMPLE/filter3.csv" "$expectedLimit1UnfilteredOutput" \
	"--limit=1 produced unexpected output even when using --filter-none!"
assertCmdEq "$CSV -j --limit=1 --filter=empty < $SAMPLE/filter3.csv" "$expectedLimit1FilteredOutput" \
	"--limit=1 produced unexpected output while using --filter!"
assertCmdEq "$CSV -j --limit=2 --filter=empty < $SAMPLE/filter3.csv" "$expectedLimit2FilteredOutput" \
	"--limit=2 produced unexpected output while using --filter!"


success
