v2.5.2 (2019-02-03)

  - clarified licensing
  - fixed build process

v2.5.1 (2018-05-22)

  - fixed documentation mistake

v2.5.0 (2018-05-07)

  - introduced option `-q none`  (ignore all quoting characters)
  - more examples
  - readme cleanup
  - Travis CI

v2.4.1 (2017-05-23)

  - fixed `--limit` when reading multi-line records (`-b`)
  - fixed `--skip` when reading multi-line records (`-b`)

v2.4.0 (2017-05-14)

  - switched default quoting style from "mixed" to "doublequotes"
  - introduced option `-q`  (set field quoting character)

v2.3.3 (2017-05-09)

  - fixed column string buffer issue

v2.3.2 (2017-04-10)

  - introduced option `-U`  (don't include unknown columns in `-n` mode)

v2.3.1 (2017-04-04)

  - fixed `-n` column name case sensitivity

v2.3.0 (2017-04-02)

  - introduced output mode `-C`  (csv output)
  - introduced option `-u`  (include unknown columns in `-n` mode)
  - fixed `-jJ` long options
  - fixed trailing separator on EOF issue
  - fixed column name escaping
  - correct semantic versioning

v2.2 (2016-11-01)

  - introduced option `--trim`  (enable line trim)
  - fixed `--skip` line length limitation
  - fixed option handling

v2.1 (2016-10-26)

  - support NUL bytes in input
  - introduced option `--color`  (set color mode)
  - removed line length limit
  - fixed column name escaping in `-m` mode

v2.0 (2016-10-15)

  - introduced output mode `-J`  (compact JSON output)
  - introduced output mode `-X`  (shell var output)
  - introduced input mode `-i`  (auto-numbered columns)
  - introduced option `-d`  (explicit separator)
  - introduced option `-M`  (don't strip BOM)
  - renamed option `-f` to `-g`  (assigned column names)
  - licensed under MIT license terms
  - code rewrite

v1.5 (2012-01-29)

  - introduced output mode `-j`  (JSON output)

v1.4 (2011-06-20)

  - introduced option `-b`  (allow breaks inside lines)
  - fixed line length issues
  - using ctype
  - build cleanup

v1.3.1 (2010-04-27)

  - fixed double separator issue

v1.3 (2008-10-28)

  - introduced input mode `-p`  (preset field names)
  - introduced option `-l`  (line limit)
  - code cleanup

v1.2 (2008-09-29)

  - introduced option `-a`  (accept only arbitrary separator)
  - code cleanup

v1.1 (2008-03-05)

  - introduced option `-F`  (always flush)
  - introduced option `-e`  (ingore errors)

v1.0 (2008-01-29)

