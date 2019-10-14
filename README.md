[//]: # (This file was autogenerated from the man page with 'make README.md')

# csv(1) - convert csv files to json and other formats

[![Build Status](https://travis-ci.org/mle86/csv-parser.svg?branch=master)](https://travis-ci.org/mle86/csv-parser)
[![MIT License](https://img.shields.io/badge/license-MIT-498e7f.svg?style=flat)](/LICENSE)


Version 2.6.0, January 2020

<pre><code><b>csv</b> [<i>MODE</i>] [<i>OPTIONS</i>] [<b>--</b>] [<i>COLUMNNAME</i>...]</code></pre>

<a name="description"></a>

# Description

This program reads CSV from its standard input (_stdin_)
and outputs the contents in a different format.

CSV (“Comma Separated Values”)
is a simple, tabular, widely-used data exchange format
mostly used for textual and numeric data.
Unfortunately, there are many different implementations.
This program aims to be fully RFC&nbsp;4180-compliant
while supporting many common standard&nbsp;violations as well.

<a name="installation"></a>

# Installation

```
$ make
# make install
```

This will compile the binary and copy it to /usr/local/bin/**csv**.
The man page will be copied to /usr/local/share/man/man.1/**csv.1.gz**.



<a name="code-standard"></a>

# Code Standard

The program source is valid ISO **C99**.
It sets the `_POSIX_C_SOURCE = 200809L` feature macro for the **getline**(3) function.
It also uses the GNU **getopt_long**(3) function.



<a name="input-modes"></a>

# Input Modes

The program operates in one of several
**input modes**
to read its input.  
The default input mode is **-a**.

* **-a, --auto-columns**  
  Automatic column names:
  Column names will be read automatically from the first CSV line.
  In this mode, _COLUMNNAME_ specifications are
  neither needed nor possible.
  (This is the default mode.)
* **-n, --named-columns**  
  Non-fixed, named columns:
  The program reads column names from the argument list
  and compares them to the first line of the CSV input,
  printing only the named columns.
  Separate column names with a single “**.**” argument!
  The name order is irrelevant, as is case.
  See the section
  “[Named Columns Mode](#named-columns-mode)”
  below
  for more information about this input mode.
* **-g, --assigned-names**  
  Names assigned to column positions:
  The program reads column names from the argument list and
  applies them to all lines.
  The special name “**@**”
  causes columns to be ignored.
  The argument order represents
  the column order in the CSV input.
  The input's first line will not be treated specially.
* **-i, --numbered-columns**  
  Numbered columns:
  In this mode,
  the columns will be automatically numbered,
  starting from zero.
  Similar to “**-g 0 1 2 3**...”,
  but will force the **-j** output mode to print each line
  as an array instead of an object.
  Not compatible with **-J** and **-C** output modes.
  In this mode,
  _COLUMNNAME_ specifications are
  neither needed nor possible.
  The first input line will not be treated specially.
* **--fixed-width**  
  See the
  “[Fixed-Width Input Mode](#fixed-width-input-mode)”
  section below.

<a name="output-modes"></a>

# Output Modes

The program operates in one of several
**output modes.**  
The default output mode is **-m**.

* **-m, --simple**  
  Simple output:
  For every input field, one line will be printed,
  with the column name, a colon and a space, and the content.
  Input lines will be separated with an extra line containing only a dash.
  Backslashes in the input will be replaced with a literal **\\\\**,
  while linebreaks in the input will be replaced with a literal **\\n**
  (if the **-b** option is given).
* **-j, --json**  
  Long JSON output:
  An ordered array of input lines is printed.
  Every line is represented by an object
  with the column names as keys and the field values as values.
  (In the **-i** input mode,
  every line is represented as an ordered array instead.)
* **-J, --compact-json**  
  Compact JSON output:
  An object with a “_columns_” and a “_lines_” key is returned.
  The “_columns_” entry is an ordered array of the column names.
  The “_lines_” entry is an ordered array of ordered arrays,
  each of which represents one input line.
* **-X, --shell-vars**  
  Output in shell&nbsp;variable&nbsp;assignment format.
  See the
  “[Examples](#examples)”
  section below.
* **-C, --csv-output**  
  Standardized CSV output.
  See the
  “[CSV output mode](#csv-output-mode)”
  section below for more details.

<a name="other-options"></a>

# Other Options


* **-d** _C_, **--separator** _C_  
  Use field separator character _C_.
  The special value “_auto_”
  causes auto-detection
  on the first input line
  (this is the default mode).
  The special value “_none_”
  disables separator character detection
  (use this for single-column input files).
* **-q** _C_, **--quotes** _C_  
  Use field quoting character _C_.
  By default, only doublequotes&nbsp;(**"**)
  may be used.
  The special value “_auto_”
  causes auto-detection
  on the first quoted field in the input
  (recognized quoting characters are "doublequotes" and 'singlequotes').
  The special value “_mixed_”
  allows the input to use both quoting characters.
  The special value “_none_”
  causes all potential enclosure characters to be read as-is.
* **-b, --allow-breaks**  
  By default,
  every record is assumed to span exactly one line.
  This option allows quoted fields to contain linebreaks,
  i.e. to span several lines.
  Without this option,
  multi-line field values will cause an error.
  This option causes
  the **--skip** and **--limit** options
  to count records instead of lines,
  which means that you cannot use **--skip** anymore
  to skip over malformed input lines
  when using the **-b** option.
* **-s** _N_, **--skip** _N_  
  Skip the first _N_ input lines.
  In modes **-n** and **-a**,
  skip the first _N_ input lines
  after the header line.
  Setting _N_ to zero means no&nbsp;skip,
  which is the default.
* **-l** _N_, **--limit** _N_  
  Stop after the _N_th input line.
  In modes **-n** and **-a**,
  the header line is not counted.
  Setting _N_ to zero removes the limit, which is the default.
* **-e, --ignore-errors**  
  Don't stop on encountering malformed CSV input;
  instead, truncate the current field value and continue.
  (By default, malformed CSV input
  terminates the program with exit status 2.)
  Malformed input includes:
    * too many columns
    * overlong fields
    * unexpected quote characters
    * unexpected end-of-line (within a quoted field)
    * unexpected end-of-file (within a quoted field)
* **--color**[=_WHEN_], **--colour**[=_WHEN_]  
  Enables or disables colorized output.
  Possible values for _WHEN_:
    * **auto**:
      Colorize output if _stdout_ is a terminal.
      This is the default setting if the option is missing.
    * **always**/**force**/**on**:
      Always colorize the output,
      even if it's redirected to a file.
      Useful if the output gets directed to a pager like **less**(1).
      This is the default if the option is given without _WHEN_.
    * **never**/**off**:
      Don't colorize the output.
* **--trim**[=_MODE_]  
  Enables or disables input whitespace trimming.
  Whitespace in this context refers only to tabs (**\\t**) and spaces.
  Possible values for _MODE_:
    * **lines**/**records**:
      Removes whitespace from the start and end of every input line.
      This is the default if the option is given without _MODE_.
    * **fields**:
      Removes whitespace from the start and end of every field value.
      Currently this is only available in **--fixed-width** input mode.
    * **none**/**off**:
      No whitespace removal.
      Whitespace
      is considered to be
      part of the input field values.
      This is the default setting if the option is missing.
* **--filter**[=_MODE_]  
  When filtering is active,
  the program won't output records
  it considers “empty”.
  (Filtered records do not count toward the **--limit** either.)
  Possible values for _MODE_:
    * **empty**/**z**:
      Drops all records
      that consist of empty-string fields only.
      This is the default if the option is given without _MODE_.
    * **blank**/**b**:
      Drops all records
      that consist of empty-string or whitespace-only fields only.
    * **zero**/**0**:
      Drops all records
      that consist of zero-valued (“0”, “.000”...) fields only.
    * **empty-or-zero**/**z0**:
      Drops all records
      that consist of zero-valued and/or empty-string fields only.
    * **blank-or-zero**/**b0**:
      Drops all records
      that consist of zero-valued and/or whitespace fields only.
    * **none**/**off**:
      No record filtering.
      This is the default setting if the option is missing.
* **-F, --flush**  
  Flush the output after every input line.
* **-M, --keep-bom**  
  Don't try to remove UTF-8/16 BOMs from the first line.
  (By default, the program will strip the BOM from the first line,
  if there is one.)
* **-u, --unknowns**  
  With this option, the **-n** mode will include all unknown columns,
  like a combination of the **-n** and **-a** modes.
  (By default, the **-n** input mode will discard all unknown columns.)
* **-U, --no-unknowns**  
  With this option, the **-n** mode will output only explicitly named columns.
  This is the default behavior.
  The option is the opposite of the **-u** option
  and has no effect in other input modes.
* **-h, --help**  
  Show command line usage information.
* **-V, --version**  
  Show program version and licensing information.

<a name="named-columns-mode"></a>

# Named Columns Mode

The **-n** input mode is useful
if the input column names are known in advance,
but not their position.  
For example,
calling the program with the arguments
“**-n phone . fax .**”
will extract the columns
“_phone_”
and “_fax_”
from the input (if they exist),
regardless of their position.
All other columns will be ignored.

The input mode also supports
**column aliases**.
This is useful if several names with the same meaning
are acceptable for one column.
To use this feature,
list the accepted column aliases
after the column name.
The&nbsp;“**.**”&nbsp;argument
separates the column aliases
from the next column definition.  
For example,
the invocation
“**-n&nbsp;phone tel telephone telno&nbsp;. fax telefax faxno&nbsp;.**”
will again extract the columns 
“_phone_”
and “_fax_”
from the input
(and will always call them thus in the output),
but will also recognize them
under one of several possible name variants.

Keep in mind that the program
will do byte-wise string comparisons
between the arguments and the input column names,
without any encoding conversion
or canonization.
The only normalization done
is lower-casing with **tolower**(3),
which only works on plain ascii letters (A..Z).

<a name="csv-output-mode"></a>

# CSV Output Mode

In CSV output mode (**-C**), the program will print valid CSV.
It tries to follow RFC&nbsp;4180 as closely as possible:
it uses a plain comma as field separator,
uses CRLF as record separator,
quotes fields with double-quotes
and escapes double-quotes in fields by doubling them,
and only quotes fields that contain a special character.
LF&nbsp;linebreaks in fields will always be printed as CRLF.

However in contrast to RFC&nbsp;4180
which mandates plain 7-bit ASCII input
and forbids use of control characters below 0x1F,
this mode will accept any encoding
and all special characters
and output everything as-is.

This mode may be useful to filter or rename columns in existing CSV files
and to change unusual separator characters in existing CSV files to the standard comma.

This mode will remove all NUL bytes from the input.

<a name="fixed-width-input-mode"></a>

# Fixed-Width Input Mode

In **--fixed-width** input mode
the program reads non-CSV flat files
according to command-line column definitions.
Every input line is assumed to span exactly one record
(option **-b** has no effect in this input mode),
there are no field quoting characters
(option **-q** has no effect),
and field separators are not needed
(option **-d** has no effect).

This mode uses position indexes
which represent bytes.
Using any multibyte encoding, such as UTF-8,
can cause significant indexing problems.
Indexing starts at 1 (one).

This mode requires the definition
of at least one column on the command line like this:

<pre><code>  <i>colname</i> <i>startpos</i>[-<i>endpos</i>] ...
</code></pre>

A missing _endpos_
means that the column ends directly before the following column definition,
or at the line's end if there is no following definition.
Single-byte columns
can be defined by setting _endpos_
to the same value as _startpos_.

For example,
the invocation
“**csv --fixed-width XA&nbsp;1-2 XB&nbsp;4-4&nbsp;XC&nbsp;5 XD&nbsp;12**”
will read
each line's first and second byte as column “XA”;
the fourth byte will be named “XB”;
the fifth to eleventh byte will be named “XC”;
and the twelfth byte and everything after it
until the line end
will be column “XD”.
The line's third byte will be ignored.

<a name="pretty-printing"></a>

# Pretty-Printing

The program has limited pretty-printing capabilities:
if the output (_stdout_) is a tty,
the output will be colorized.
Indentation will not be changed in any way.
(See the **--color**&nbsp;option
to force pretty-printing if _stdout_ is not a&nbsp;tty
or to disable it completely.)

* Column names will be printed in green.
* Structural elements,
  such as record separators,
  will be printed in yellow.
* Escaped characters
  will be printed in cyan.
* Everything else,
  including the field contents,
  will be printed in the terminal's default color.


<a name="examples"></a>

# Examples

All examples assume this CSV file as input:

<pre><code>h1;h2;h3
c1;c2;c3
ca;"cb""";cc
cx;cy;cz
</code></pre>

The default input mode is **-a**
(get column names automatically from first line).
The default output mode is **-m**
(simple output).
This is what is looks like:

**`csv < test.csv`**

<pre><code>h1: c1
h2: c2
h3: c3
-
h1: ca
h2: cb"
h3: cc
-
h1: cx
h2: cy
h3: cz
</code></pre>

In contrast, this is what the **-j** (JSON) output mode
looks like with the same input:

**`csv -j < test.csv`**

<pre><code>[{"h1":"c1","h2":"c2","h3":"c3"}
,{"h1":"ca","h2":"cb\"","h3":"cc"}
,{"h1":"cx","h2":"cy","h3":"cz"}]
</code></pre>

The **-J** output mode (compact JSON)
only prints the column names once:

**`csv -J < test.csv`**

<pre><code>{"columns": ["h1","h2","h3"],
"lines": [
 ["c1","c2","c3"]
,["ca","cb\"","cc"]
,["cx","cy","cz"]]}
</code></pre>

In the **-i** input mode,
the first line is not considered special.
Instead, all columns are automatically numbered:

**`csv -i < test.csv`**

<pre><code>0: h1
1: h2
2: h3
-
0: c1
1: c2
2: c3
-
0: ca
1: cb"
2: cc
-
0: cx
1: cy
2: cz
</code></pre>

As a special case,
the long JSON mode&nbsp;(**-j**) omits the auto-numbered column names
of the **-i** input mode.
Thus,
an array
instead of an object
will be printed
for every record,
simplifying further JSON processing.

**`csv -i -j < test.csv`**

<pre><code>[["h1","h2","h3"]
,["c1","c2","c3"]
,["ca","cb\"","cc"]
,["cx","cy","cz"]]
</code></pre>

Finally, the **-X** output mode
generates shell variable assignments
like this:

**`csv -X < test.csv`**

<pre><code>CSV_COLNAME_0=h1
CSV_COLNAME_1=h2
CSV_COLNAME_2=h3
CSV_0_0=c1
CSV_0_1=c2
CSV_0_2=c3
CSV_1_0=ca
CSV_1_1="cb\""
CSV_1_2=cc
CSV_2_0=cx
CSV_2_1=cy
CSV_2_2=cz
CSV_RECORDS=3
</code></pre>

The varname scheme is
“CSV\__lineno_\__fieldno_”,
with lines and fields counted from zero.

Again, there's a special case for the **-i** input mode,
because it has no relevant column names:
The “CSV\_COLNAME\__colno_” assignments will be omitted,
and the first input line is read as a regular record.

**`csv -i -X < test.csv`**

<pre><code>CSV_0_0=h1
CSV_0_1=h2
CSV_0_2=h3
CSV_1_0=c1
CSV_1_1=c2
CSV_1_2=c3
CSV_2_0=ca
CSV_2_1="cb\""
CSV_2_2=cc
CSV_3_0=cx
CSV_3_1=cy
CSV_3_2=cz
CSV_RECORDS=4
</code></pre>


<a name="input"></a>

# Input


<a name="encoding"></a>

### Encoding

The program is encoding-agnostic,
so long as
a single-byte character is used as field separator
and LF&nbsp;(**\\n**) or CRLF&nbsp;(**\\r\\n**) is used as record separator.

NB:
Some encodings,
like UTF-16 and UTF-32,
may use CR/LF bytes
to represent other characters.
This will result in unexpected output,
since the program is not wide&nbsp;character-aware.
Plain ASCII or UTF-8 are safer choices,
because they never use a low byte
for the representation of another character.

It is recommended that input
in an encoding
other than plain ASCII or UTF-8
be converted to UTF-8
with a tool like **iconv**(1)
before being passed to this program.

<a name="separator-character"></a>

### Separator Character

By default,
the program will look for a field separator character
in the first input line;
the first such character found there
will be used for the rest of the input.
Accepted separator characters are
comma&nbsp;(**,**),
semicolon&nbsp;(**;**),
tabulator&nbsp;(**\\t**),
and
pipe&nbsp;(**|**).
If the input uses a different separator character,
specify it manually with the **-d** option.

Both the auto-detection feature
and the **-d** option
support one-byte separators only
and assume that the input
does not use more than one distinct separator.
Input files using multiple different separators
and/or multibyte separators
cannot be parsed correctly.

<a name="quoting"></a>

### Quoting

By default, this program recognizes double-quotes (**"**)
as the field enclosure character.
This can be changed with the **-q** option
if your input uses a different quoting character.
The quoting character
will be removed from the output.
Fields need not be quoted,
except for multiple-line values
(see option **-b**)
or fields containing the separator or quoting character.
If the quoting character
is present inside a quoted field,
it must be doubled.

For example,  
**"field""with""quotes"**
will be interpreted as **field"with"quotes**,
whereas  
**field""without""quotes"**
will be interpreted as-is.

The special setting **-q&nbsp;mixed**
allows input files
to use both single-quotes (**'**)
and double-quotes (**"**)
as field enclosures.

<a name="single-column-csv-files"></a>

### Single-Column CSV Files

Single-column files can be used as input,
but the separator auto-detection
(in case of “**-d auto**”
or no **-d** argument at all)
will fail and terminate the program
with exit&nbsp;code&nbsp;3.

To correctly read single-column files,
use “**-d none**”,
which tells the program
that the input does not contain any actual separator characters.

Alternatively, use the **-e** option,
as a missing separator character can be ignored;
however, this will cause malformed CSV input to be ignored as well.

<a name="multi-line-column-names"></a>

### Multi-Line Column Names

If the first input field contains linebreaks,
the automatic separator&nbsp;detection
won't be able to find the separator
and terminate the program
with exit&nbsp;code&nbsp;3,
as it only checks the first input line.
In this case, it is necessary
to manually specify the separator character
with the **-d**&nbsp;option.

<a name="exit-codes"></a>

# Exit Codes

By default,
the program will terminate with exit code zero (success).
Other exit codes
signify various error conditions:

* **1**
  Syntax error
  (invalid arguments, missing, or too many column names).
* **2**
  Input format error
  (too many columns,
  no column match,
  empty header line,
  unexpected quote/EOL/EOF).
  Note that when the program exits with code&nbsp;2,
  it will already have printed parts of the CSV contents.
* **3**
  No separator found on first line.
* **4**
  Empty input.
  (This is only an error in input modes **-n** and **-a**,
  whereas input modes **-g** and **-i** can handle empty input gracefully.)
* **10**
  Internal error
  (out of memory or getline(3) read error).

<a name="license"></a>

# License

[MIT](https://opensource.org/licenses/MIT)

<a name="author"></a>

# Author

Maximilian Eul &lt;[maximilian@eul.cc](mailto:maximilian@eul.cc)&gt;
(https://github.com/mle86)

<a name="standards"></a>

# Standards

Y.&nbsp;Shafranovich,
_Common Format and MIME Type for Comma-Separated Values (CSV) Files_,
[RFC&nbsp;4180](https://tools.ietf.org/html/rfc4180),
October 2005.

T.&nbsp;Bray,
_The JavaScript Object Notation (JSON) Data Interchange Format_,
[RFC&nbsp;7159](https://tools.ietf.org/html/rfc7159),
March 2014.
