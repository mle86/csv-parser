# More `csv` Examples


## Counting Records

If your CSV file contains only single-line records,
a simple “`wc -l file.csv`” will do as it does for all files.
This of course includes the header line.

If your CSV file contains multi-line records or if you are not sure,
try this command:

`csv -jb < file.csv | wc -l`

The `-j` mode outputs exactly one line per CSV record
(not counting the header line),
so this command correctly outputs the number of records.
If you want that count to include the header line without manually adding 1,
add the `-i` option.

Alternatively,
you can use a construction such as
“`eval $(csv -Xb < file.csv | tail -n1)`”,
then use the `$CSV_RECORDS` environment variable.
The `-X` output mode
always prints a final <code>CSV\_RECORDS=<i>N</i></code> assignment.


## Header Extraction

To extract only the column names from a CSV file,
use this command:

`csv -jibl1 < file.csv`

- The `-j` option turns on JSON output.
- The `-i` option causes the program to treat the first line (the header) like a regular record with numbered columns.
- The `-b` option turns on multi-line support -- column names with linebreaks are uncommon but possible.
- The `-l1` option (short for `--limit=1`) limits the output to the first record (the header, in this case).

The output will look like this:

`[["colname1", "colname2", …]]`


## Syntax Check

To verify the syntax of an unknown CSV file,
use this command and test its exit code:

`csv -b < file.csv >/dev/null 2>/dev/null`

It will exit with one of these codes:

- 0: The file looks valid.
- 2: The file's syntax is invalid.
- 3: The first line contains no recognized separator character.  
  (Add the `-d none` option if you do not consider that an error.)
- 4: The file is empty.  
  (Add the `-i` option if you do not consider that an error.)

