# WH: working notes

`lib/WH/wh.cpp`, the SDK's wireless sample wrapper in the main program. `docs/binary.md` ("WH, the wireless wrapper",
"Compiler") has the results; this is how they were reached, and the open question of `.data`.

## The compiler, from the `.bss` addressing

- With `-ipa file`, mwccarm pools a file's statics into one section and addresses them from its start. Without it,
  each static is its own symbol. WH's small statics are one pool in the ROM (`0x02138F40`, `sSysState` at +0x2C), so
  WH had `-ipa file`.
- Arrays of `char` are never pooled (tested: `u8`, `s8` and `char` arrays each get a section of their own; `u16` and
  `u32` arrays of the same size are pooled).
- Builds up to 3.0 build 137 (2.0 SP2 patch 2) pool every object. Builds 138 and 139 pool objects of up to 60 bytes
  and keep those of 64 and more apart (test files of 36, 44, 48, 52, 56, 60 and 64 bytes). The ROM addresses the
  64-byte `sParentParam` on its own and the 32-byte `sScanParam` from the pool, which fits 138 and 139 only.

## The data sort

Measured with test files under build 138 (`-ipa file`), and checked against compiled WH:

- The compiler keeps one list of a file's data objects, `.bss` ones included, and heap-sorts it by size (a plain
  max-heap sort, ascending; a parent is swapped with a child only when the child is strictly larger, the left one
  first). Sections are emitted in that order, the pool at the place of its first member.
- The list is the objects newest first. A table's strings are created before the table, so a table and its strings
  appear in the list as the table, then its strings from the last to the first.
- In C, a static without an initialiser is a tentative definition and is only created at the end of the file, in
  reverse order of declaration (so it heads the list, in declaration order). A static with `= 0` is created where it
  is declared, and is still in `.bss`. In C++ every static is created where it is declared.
- An unreferenced `static` (a table in an unused static function included) is dropped before the sort and does
  not change it. The strings of the code are not in the list: they follow the sorted objects in the order the code
  uses them.

This model reproduces every compiled arrangement tried exactly (the strings' order and the pool's), C and C++.

The ROM's `.data` has the three tables (the WH state names 40 bytes, the error names 92, the state code names 108)
after their 60 strings, all in order of size, and then the code's strings in order of use. What decides the order
of the strings of equal length is the whole list. Searched with the model (an annealing over arrangements, scored
on the strings' places):

- In C (each static tentative or `= 0`, any order, the three tables anywhere): at best 56 of 60, all the best
  runs missing the same four 25-byte strings by one place. Adding objects that the link would have stripped (the
  sample's `WMDataSet`, a table of WM state names, the table of API names, small ghosts) did not help.
- In C++ (every static created where declared): 60 of 60, with the statics first and then the three tables in the
  order error names, state code names, WH state names. That is the natural form: the statics at the top of the
  file, and each table a static of the function that uses it (`WH_GetWMErrCodeName`, `WH_GetWMStateCodeName`,
  `WH_ChangeSysState`, in that order in `.text`). The statics' order is the one the search found, with names
  swapped among statics of one size to give the pool's places, which the strings do not depend on.

So the file is C++ here. One static is not one: `gWHChildSsid` (`0x02138FB4`) is read by overlays 109 and 110, so
it is global (`dsd` refuses a local symbol that another module refers to).

The tools used are not kept: a simulation of the sort (the heap sort above over the list built as described) and
the annealing search.
