# moonlander-layout

This repository contains the QMK keymap `qgmlwy` for a ZSA Moonlander MK1 (rev A).
QMK builds the keymap as an
[external userspace](https://docs.qmk.fm/newbs_external_userspace) with
[ZSA's QMK fork](https://github.com/zsa/qmk_firmware). The file
[`src/qmk/.qmk-ref`](src/qmk/.qmk-ref) contains the commit of the fork that the
build uses.

## Layout

| Directory                 | Contents                                         |
| ------------------------- | ------------------------------------------------ |
| `src/qmk/`                | The QMK userspace and the keymap                 |
| `src/check_standards/`    | The standards checker                            |
| `src/key_log_capture/`    | The key log capture for Windows                  |
| `src/key_log_converter/`  | The key log converter                            |
| `test/check_standards/`   | The checker tests, one directory for each case   |
| `test/key_log_converter/` | The converter tests, one directory for each case |
| `test/qmk/`               | The keymap unit tests                            |
| `.build/`                 | The build output, which git ignores              |

## Installation

The setup script is for Ubuntu, and for Ubuntu on WSL. The script uses
`sudo apt-get`.

To install the tools, run this command in the root directory of the repository:

```bash
./setup.sh
```

The script installs these items:

- The QMK CLI and the QMK toolchain
- ZSA's `qmk_firmware` in `~/qmk_firmware`, with `src/qmk/` as the QMK userspace
- Node at the version in [`versions.env`](versions.env), and the npm devDependencies
- pre-commit and its git hooks
- The check tools: shellcheck, tree-sitter, clang-tidy and Doxygen
- gcovr for the test coverage reports
- clang 18 and [Mull][mull] for the mutation tests
- CMake, and cmocka for the unit tests
- The MinGW-w64 cross compiler, for the Windows build of the key log converter

Then the script builds the firmware one time.

## Build and flash

To build and flash the firmware, do these steps:

1. Run `cmake --workflow --preset firmware`. It prints the name of the new file.
2. Flash that file from `.build/` with [Keymapp][keymapp].

The name of each firmware file ends in the commit of the build, for example
`zsa_moonlander_reva_qgmlwy-2bcdfa9.bin`. If `src/qmk/` has changes with no commit,
the name also ends in `-dirty-` and the first 8 digits of the SHA-256 hash of the file.
The build keeps the older firmware files in `.build/`. To remove all the build output,
also the firmware files, delete `.build/`.

The firmware of a PR build in CI has the PR number and the commit in its name, for
example `zsa_moonlander_reva_qgmlwy-pr117-1243469.bin`. CI sets the environment
variable `FIRMWARE_SUFFIX`, which replaces the commit in the name.

CMake starts the QMK CLI, and QMK builds the firmware with its own build. The
`firmware` preset builds in `.build/firmware/` and needs only CMake and the QMK tools.
QMK uses one job for each processor. To use a different number of jobs, set the CMake
cache variable `QMK_JOBS`, for example `cmake --preset firmware -DQMK_JOBS=4`.

QMK writes the object files of the firmware into `.build/qmk/normal/`, and the object
files of the debug firmware into `.build/qmk/debug/`. Thus, the two builds do not
share object files, and the builds of different work trees do not use the same
directory in `qmk_firmware`. A QMK build that writes a compile database also deletes
`qmk_firmware/.build/`, but no build of this repository uses that directory.

This command builds the firmware and the debug firmware, and writes the compile
databases:

```bash
cmake --workflow --preset compile-databases
```

`.build/compile_commands.json` is the compile database of the firmware, with an entry
for `keymap.c`. `.build/cmake/compile_commands.json` is the compile database of the
host programs and the unit tests. For IntelliSense in VS Code, set the
`C_Cpp.default.compileCommands` setting to these two files.
`.build/debug/compile_commands.json` is the compile database of the debug firmware,
for the static analysis.

## Profiling debug build

The profiling debug build measures the time that the keymap code takes on the
keyboard. It writes the results on the QMK console, as the key event log does. The
normal firmware does not contain the profiling code.

To build the debug firmware, run this command:

```bash
cmake --workflow --preset debug-firmware
```

The name of the debug firmware file is the name of the normal firmware file with
`-debug` at the end, for example `zsa_moonlander_reva_qgmlwy-2bcdfa9-debug.bin`.
Flash that file from `.build/` with [Keymapp][keymapp]. To go back to the normal
firmware, run `cmake --workflow --preset firmware` and flash its file.

The debug build is a CMake target and a workflow preset, not a make variable. The
`Makefile` contains only aliases, with no variables. The target gives the debug
firmware its own file name and its own build directory, and CI builds the target
with the other targets. The debug build always compiles all files, because it also
writes the compile database of the debug firmware.

### Toggle key

In the debug firmware, the key at (3,0) switches profiling on and off. It is the
leftmost key of the bottom letter row of the left half. In the normal firmware, this
key does nothing. Profiling is off when the keyboard starts. When profiling goes on, all
the values go back to zero.

### Profile records

While profiling is on, the keyboard writes the records in this table. Each record is
one line, with decimal numbers. `<time>` is the time since power-up in ms. The key
log capture also saves these records.

| Record                                               | When                    |
| ---------------------------------------------------- | ----------------------- |
| `KL,C,<time>,<cpu_hz>`                               | When profiling goes on  |
| `KL,P,<time>,<name>,<calls>,<cycle_sum>,<cycle_max>` | Each 5 s, for each call |
| `KL,R,<time>,<scan_count>,<interval_ms>`             | Each 5 s, after `KL,P`  |
| `KL,H,<time>,<stack_free_bytes>`                     | Each 5 s, after `KL,R`  |

- `KL,C` gives the core clock in Hz.
- `KL,P` gives the calls, the sum of their cycles and the cycles of the slowest call.
- `KL,R` gives the matrix scans and the length of the interval in ms.
- `KL,H` gives the lowest number of free bytes on the main stack since power-up.

The values of `KL,P` and `KL,R` are for the last interval of 5 s only.

The measured calls are `record_user`, which is all of `process_record_user()`, and
each record handler in call order: `key_log`, `plain_mode`, `case_modes`,
`bracket_pairs`, `special_mode`, `symbol_holds` and `precision_key`.

The cycle counter of the Cortex-M4 core gives the cycles. Each value also contains
the cost of the measurement. A call of an empty handler gives 7 to 15 cycles. The
cycles of `record_user` also contain the measurement of each record handler that it
calls, about 100 cycles for each handler.

## Memory report

At the link step, each firmware build shows the use of each memory region. QMK shows
each output of the linker as a warning, thus the link step shows `[WARNINGS]`. The
ChibiOS linker script gives all free RAM to the heap, thus the linker shows `ram0` at
100%.

gcc writes the stack use of each function into a `.su` file. The build copies these
files into `.build/stack-usage/normal/` and `.build/stack-usage/debug/`.

This command builds the firmware and the debug firmware, and then shows the memory
report:

```bash
cmake --workflow --preset memory-report
```

The `memory-report` target only reads the output of the last builds, and writes the
report into `.build/memory-report.txt`. For each build, the report shows these items:

| Item                 | Source                                                 |
| -------------------- | ------------------------------------------------------ |
| Flash and RAM use    | The symbols of the ChibiOS linker script in the `.elf` |
| Largest symbols      | The first 20 lines of `arm-none-eabi-nm --size-sort`   |
| Largest stack frames | The 20 largest stack frames in the `.su` files         |

The RAM use does not include the heap. The type of a symbol tells its contents. Type
`T` or `t` is code, `R` or `r` is constant data, `D` or `d` is data with an initial
value, and `B` or `b` is data that starts at zero. The kind of a stack frame tells if
its size changes. Kind `static` is a fixed size. Kind `dynamic` changes at run time,
and `dynamic,bounded` has a maximum.

A stack frame is the stack use of one function. It does not include the functions
that the function calls.

## Host programs

[CMake][cmake] builds the standards checker, the key log converter and the keymap unit
tests, and runs the static analysis. Each CMake preset in `CMakePresets.json` has its
build directory in `.build/`:

| Configure preset | Parts                             | Build directory         |
| ---------------- | --------------------------------- | ----------------------- |
| `dev`            | All parts and the static analysis | `.build/cmake/`         |
| `firmware`       | The firmware                      | `.build/firmware/`      |
| `checker`        | The standards checker             | `.build/checker/`       |
| `converter`      | The key log converter             | `.build/converter/`     |
| `unit-tests`     | The keymap unit tests             | `.build/unit-tests/`    |
| `windows`        | The key log converter for Windows | `.build/windows/`       |
| `coverage`       | The tests, with gcov counters     | `.build/coverage/`      |
| `mutation`       | The tests, with Mull mutants      | `.build/mutation/`      |
| `mutation-diff`  | Mutants of the changes only       | `.build/mutation-diff/` |

The configure step writes the compile database `compile_commands.json` into the build
directory. Each workflow preset does the configure step and the build of one configure
preset. The test workflow presets also run the tests. For example, this command builds
all parts in `.build/cmake/` and runs all the tests:

```bash
cmake --workflow --preset test
```

This command builds the firmware, the debug firmware, the compile databases, the host
programs and the unit tests in `.build/cmake/`, and then runs the static analysis:

```bash
cmake --workflow --preset all
```

To see all the presets, run this command:

```bash
cmake --list-presets=all
```

The checker, the converter and the unit tests use the same strict compiler flags,
from [`CMakeLists.txt`](CMakeLists.txt). Each warning stops the build.

CMake finds tree-sitter, cmocka and MinGW-w64 in the system directories. To use a
different directory, for example a `dpkg -x` extraction of the packages, set
`CMAKE_PREFIX_PATH` to its `usr` directory.

## Make aliases

CMake and CTest do all the builds, the tests and the checks. The `Makefile` in the
root directory only gives short aliases for the commands in a terminal. Each alias
runs one command:

| Alias                   | Command                                       |
| ----------------------- | --------------------------------------------- |
| `make`, `make firmware` | `cmake --workflow --preset firmware`          |
| `make debug-firmware`   | `cmake --workflow --preset debug-firmware`    |
| `make memory-report`    | `cmake --workflow --preset memory-report`     |
| `make build`            | `cmake --workflow --preset build`             |
| `make analyze`          | `cmake --workflow --preset analyze`           |
| `make all`              | `cmake --workflow --preset all`               |
| `make test`             | `cmake --workflow --preset test`              |
| `make checker-test`     | `cmake --workflow --preset checker-test`      |
| `make converter-test`   | `cmake --workflow --preset converter-test`    |
| `make qmk-test`         | `cmake --workflow --preset qmk-test`          |
| `make windows`          | `cmake --workflow --preset windows`           |
| `make coverage`         | `cmake --workflow --preset coverage`          |
| `make mutation`         | `cmake --workflow --preset mutation`          |
| `make mutation-diff`    | `cmake --workflow --preset mutation-diff`     |
| `make compiledb`        | `cmake --workflow --preset compile-databases` |
| `make clean`            | `cmake --build --preset dev --target clean`   |
| `make help`             | `cmake --list-presets=all`                    |

The `build` workflow builds the firmware, the debug firmware, the compile databases,
the host programs and the unit tests, writes the memory report, and runs no check.
The `clean` target of the `dev` preset removes the build output of `dev` and the
compile databases of the firmware and the debug firmware.

## Key log capture

The key log capture saves the QMK console of the keyboard in log files on a Windows
computer. It uses [hid_listen][hid-listen] from PJRC. `versions.env` pins its version
and the SHA-256 hash of its zip file.

The log files contain all the text that you type while logging is on. Keep the log
files only on the local computer.

### Install the capture

On each Windows computer, run this command one time in WSL, in the root directory of
the repository:

```bash
src/key_log_capture/install-key-log-capture.sh
```

The script does these steps:

- It downloads hid_listen from PJRC and makes sure that the hash is correct.
- It copies `hid_listen.exe` and `start-key-log-capture.ps1` to the program folder.
- It adds the task `moonlander-key-log-capture` to the Windows Task Scheduler.
- It starts the task one time.

The program folder is `%LOCALAPPDATA%\moonlander-key-log\`. The task is only for your
Windows user, and it starts at each logon. The script does not need administrator
rights.

To install a new version, run the script again.

### Log files

The log files are in `%LOCALAPPDATA%\moonlander-key-log\logs\`. Each start of the
task makes a new file. The name of the file contains the start time, for example
`keylog-2026-11-06T0812.log`. The file contains the console text of the keyboard with
no change, but hid_listen writes each line end as CR LF. hid_listen also writes its
status lines into the file, for example `Waiting for device:`.

When you disconnect the keyboard and connect it again, hid_listen continues in the
same file.

### Remove the capture

To remove the task and the program files, run this command in WSL:

```bash
src/key_log_capture/uninstall-key-log-capture.sh
```

The script does not delete the log files.

## Checks

To run the checks, use these commands:

| Command                                    | Result                                |
| ------------------------------------------ | ------------------------------------- |
| `pre-commit run --all-files`               | Runs the hooks.                       |
| `cmake --workflow --preset all`            | Does the build and then the analysis. |
| `cmake --workflow --preset analyze`        | Runs the static analysis.             |
| `cmake --workflow --preset checker-test`   | Tests the standards checker.          |
| `cmake --workflow --preset qmk-test`       | Runs the keymap unit tests.           |
| `cmake --workflow --preset converter-test` | Tests the key log converter.          |
| `cmake --workflow --preset coverage`       | Measures the test coverage.           |
| `cmake --workflow --preset mutation-diff`  | Tests the tests of the changes.       |

The static analysis reads the compile databases and builds no program. The keymap
checks read the compile databases of the firmware and of the debug firmware, so run
the `compile-databases` or the `build` workflow before the `analyze` workflow. The
`all` workflow does the build and then the analysis. The analysis runs these checks,
and each check is a CMake target:

| Target                    | Check                                                   |
| ------------------------- | ------------------------------------------------------- |
| `clang-tidy-keymap`       | clang-tidy on the keymap                                |
| `clang-tidy-keymap-debug` | clang-tidy on the keymap of the debug build             |
| `clang-tidy-checker`      | clang-tidy on the standards checker                     |
| `clang-tidy-converter`    | clang-tidy on the key log converter                     |
| `clang-tidy-unit-tests`   | clang-tidy on the keymap unit tests                     |
| `doxygen`                 | Doxygen on the keymap, the checker and the converter    |
| `gcc-strict`              | gcc with more warnings on the keymap                    |
| `gcc-strict-debug`        | gcc with more warnings on the keymap of the debug build |

The analysis continues after a failed check and shows each failed check. Each check
prints its duration in a line that starts with `timing`. To run one check, run its
target in the build directory of the `dev` preset. For example, these commands do the
configure step and run clang-tidy on the standards checker:

```bash
cmake --preset dev
cmake --build --preset dev --target clang-tidy-checker
```

The hooks are editorconfig-checker, clang-format, Prettier, shellcheck, shfmt,
actionlint and the standards checker. Git runs these hooks at each commit. The
standards checker hook builds the checker with the `checker` preset.

The QMK build uses `-Wall -Werror`. clang-tidy and the `gcc-strict` check add more
compiler warnings for the keymap files, but not for the QMK files. The `gcc-strict`
check compiles the keymap files again with the QMK gcc. In that compilation, the QMK
headers are system headers, thus gcc shows no findings in them. The checks that end
in `-debug` read the compile database of the debug firmware. Only that database has
the profiling sources, and only in that database `DEBUG_PROFILE` is set.

The standards checker has two rules for the `.c`, `.h` and `.inc` files:

- `file-docstring`
    - Each file starts with a Doxygen `/** @file` block.
    - The block has a description.
- `nesting`
    - Control flow in a function has a maximum of two levels.
    - An `if` or a `case` contains no control flow.

CI runs all the checks on each PR and on each push to `main`. A PR can merge only if
these CI checks pass:

- `Build` builds the firmware files, the compile databases, the programs and the tests.
- `Test` runs all the CTest tests on the programs from the `Build` job.
- `Analyze` runs the static analysis on the compile databases from the `Build` job.
- `Lint` runs the hooks.
- `Validate PR title` checks the PR title.

The `Build` job runs one time in each CI run. It also builds the key log converter for
Windows, and shows the memory report in its job summary. The artifact `firmware`
contains the firmware, and the artifact `debug-firmware` contains the debug firmware.
The `Test` and `Analyze` jobs start when the `Build` job is complete, and they run at
the same time. If a PR changes no file that the build, the tests or the analysis
use, CI skips these three jobs. A skipped job passes its check.

CI also runs the Test coverage check on each PR and on each push to `main`, and the
Mutation safety gate check on each PR.

## Unit tests

The keymap unit tests use [cmocka][cmocka] and [CTest][ctest]. The test build compiles
the keymap sources on the host, with the keycode and type headers of ZSA's
`qmk_firmware`. The QMK CLI gives the `qmk_firmware` directory (`qmk env QMK_HOME`).
To use a different directory, set the `QMK_HOME` environment variable.

The profiling tests compile the keymap sources a second time, with `DEBUG_PROFILE`
defined, as the profiling debug build does. A fake gives the cycles, because the host
has no cycle counter.

`cmake --workflow --preset qmk-test` builds the tests in `.build/unit-tests/` and runs
them. `cmake --workflow --preset test` builds and runs them in `.build/cmake/`, with
the checker and converter tests.

Each cmocka case is one CTest test. Each case directory of the checker tests and of
the converter tests is also one CTest test. Such a test runs in its own folder in the
build directory, with a copy of its input files. A failed checker or converter test
shows the diff of each wrong output. The labels `qmk`, `checker` and `converter`
select the tests of one part. After `cmake --workflow --preset test`, ctest with the
`dev` preset can get more arguments. For example, these commands run only the tests
with `keymap_base` in their name, and then only the checker tests:

```bash
ctest --preset dev -R keymap_base
ctest --preset dev -L checker
```

The CMake presets give the build directory and the compile database
`.build/cmake/compile_commands.json`. Thus, tools that read CMake presets can show,
run and debug each test.

The static analysis also runs clang-tidy on the test code (`clang-tidy-unit-tests`).

## Test coverage

[gcovr][gcovr] makes the test coverage reports from the gcov counters of GCC. This
command builds the host programs and the unit tests with the counters in
`.build/coverage/`, runs all the tests and then runs the coverage gate:

```bash
cmake --workflow --preset coverage
```

The coverage includes only the code under test: the keymap in `src/qmk/keyboards/`,
the standards checker and the key log converter. It does not include the test code,
the fake QMK functions, the QMK headers and the tree-sitter grammar.

The coverage build uses the strict compiler flags, but not `-fanalyzer`. The GCC
analyzer reads the code of the gcov counters as out-of-bounds accesses. The other GCC
builds run the analyzer.

The `coverage-report` target writes the reports into `.build/coverage/report/`:

| File            | Contents                                         |
| --------------- | ------------------------------------------------ |
| `index.html`    | The coverage of each line and branch, for people |
| `coverage.info` | The LCOV tracefile                               |
| `cobertura.xml` | The Cobertura report                             |
| `coverage.json` | The gcovr data that the gate reads               |

The target reads the counters and then deletes them. Thus, each report shows only
the tests that ran after the report before it.

The `coverage-gate` target compares the line coverage and the branch coverage of each
area with a minimum. The areas are the keymap, the standards checker and the key log
converter. `add_coverage_area` in `CMakeLists.txt` gives the minimums. The gate prints
the coverage of each area, writes it into `<area>.md` in the report folder, and fails
if a value is less than its minimum. When the tests get better, increase the minimum.

The Test coverage workflow in CI runs `cmake --workflow --preset coverage` on each PR
and on each push to `main`. The job summary shows the coverage of each area, and the
artifact `coverage-report` contains the report folder.

The CMake Tools extension of VS Code can show the coverage of each line after it runs
the tests with coverage. To use it, select the `coverage` configure preset. Then set
`cmake.postRunCoverageTarget` to `coverage-report`, and set `cmake.coverageInfoFiles`
to `${workspaceFolder}/.build/coverage/report/coverage.info`.

## Mutation testing

[Mull][mull] makes small changes in the code under test. Each change is a mutant. Mull
runs the tests for each mutant. If a test fails, the test kills the mutant. If all
tests pass, the mutant survives, thus the tests do not check the behaviour that the
mutant changes.

The mutation tests include the same code as the test coverage. `src/ci/mull.yml` gives
the code under test and the mutation operators.

Mull adds the mutants while clang compiles the code. Thus, the mutation presets build
with clang 18 in their own build directories. These builds use the strict compiler
flags of the GCC builds, but not the flags that only GCC has: `-Wduplicated-cond`,
`-Wduplicated-branches`, `-Wlogical-op`, `-Wjump-misses-init` and `-fanalyzer`. The
builds also use AddressSanitizer and UndefinedBehaviorSanitizer. Thus, a mutant that
reads outside an object or causes undefined behaviour stops the test.

| Workflow preset | Mutants                          | Result                     |
| --------------- | -------------------------------- | -------------------------- |
| `mutation`      | All the mutants                  | A report only              |
| `mutation-diff` | The mutants on the changed lines | Fails if a mutant survives |

The `mutation-diff` preset compares the work tree with `origin/main`. To compare with a
different git ref, set the cache variable `MUTATION_DIFF_REF`. For example, these
commands compare with the last commit:

```bash
cmake --preset mutation-diff -DMUTATION_DIFF_REF=HEAD
cmake --build --preset mutation-diff
```

The `mutation` target runs Mull one time for each program: the standards checker, the
key log converter and each unit test program. Each run uses the CTest tests of its
program. Then `mull-reporter` merges the results. If a test of one program kills a
mutant, the mutant is killed. The target writes these files into `report/` in the
build directory:

| File              | Contents                                         |
| ----------------- | ------------------------------------------------ |
| `mutation.txt`    | The surviving mutants                            |
| `mutation.html`   | The Mutation Testing Elements report, for people |
| `mutation.json`   | The Mutation Testing Elements data               |
| `mutation.sqlite` | The Mull data of all the runs                    |

### Tests that write files

A mutant of a program that writes files can change the path of an output file. For
example, a mutant can make a path buffer too short. The program then writes to the
first part of the path. If the path is absolute, that part can be a folder outside the
build directory, for example the repository root.

Thus, each checker test and each converter test runs in its own folder in the build
directory, for example `test/key_log_converter/<case>/`. The test copies its input
files into that folder, at their paths from the repository root. The program gets only
relative paths. The converter writes its outputs into
`test/key_log_converter/<case>/output/` in that folder. The first part of a relative
path is also in that folder. A new test of a program that writes files must also give
the program only relative paths.

### Mutation safety gate

The Mutation safety gate workflow runs on each PR to `main`. The PR checkout is a merge
commit, and its first parent is the base branch. The gate runs only the mutants on the
lines that are different from the first parent, thus only the mutants of the PR. Each
surviving mutant fails the gate. A PR that does not change the code under test has no
mutants and passes.

The job summary shows each surviving mutant, and an annotation shows it at its line
in the PR. The artifact `mutation-report` contains the report folder.

### Fix a surviving mutant

The log and `mutation.txt` show each surviving mutant in this format:

```text
.../case_modes.c:83:28: warning: Survived: Replaced <= with < [cxx_le_to_lt]
    if (key >= KC_A && key <= KC_Z) {
```

The first line gives the path of the file, the line and the column of the mutant, the
change, and the mutation operator. The output shows the full path, but the example
shows only the file name. The next line shows the source line. Then a line with a `^`
shows the column.

To fix a surviving mutant, do these steps:

1. Find the behaviour that the change breaks.
2. Write a test that checks that behaviour.
3. Make the change by hand, and make sure that the test fails. Then remove the change.
4. Run `cmake --workflow --preset mutation-diff` again.

Do these steps again until no mutant survives. In the example, the mutant makes Z a
key that is not a letter. No test types Z in a case mode, thus no test sees the
change. A test that types Z in a case mode kills the mutant.

If no test can see the change, the mutant is equivalent: the changed code has the same
behaviour. An equivalent mutant often shows code that has no function, for example a
condition that is always true. Remove that code. If the code must stay, put the
comment `// mull-ignore-next: <operator>` on the line above it. The review of the PR
then shows this decision.

Mull does not remove a call through a function pointer, for example a call of a
handler from a table. Thus, a `cxx_remove_void_call` mutant of such a call always
survives. Put `// mull-ignore-next: cxx_remove_void_call` on the line above that call.

A mutant on a line that no test runs also survives. The coverage report shows that
line as not run.

## Key log converter

The key log converter makes a text corpus and key metrics from the key event logs of
the keyboard. It merges one or more log files into one set of output files. The output
files are the input of [carpalx][carpalx], of the `ngrams` tool of
[keyboard_layout_optimizer][keyboard-layout-optimizer] and of the
[QMK heatmap tool][qmk-heatmap].

To build the converter `.build/converter/key_log_converter`, run these commands:

```bash
cmake --preset converter
cmake --build --preset converter
```

To build the Windows program `.build/windows/key_log_converter.exe`, run this
command:

```bash
cmake --workflow --preset windows
```

The first argument is the output folder, and the folder must exist. The other arguments
are the log files. For example:

```bash
.build/converter/key_log_converter ~/converted keylog-2026-11-06T0900.log
```

The output files contain the text that you typed, also passwords and other private
text. Keep the output folder outside of all git work trees.

The converter writes these files into the output folder:

| File              | Contents                                                       |
| ----------------- | -------------------------------------------------------------- |
| `corpus.txt`      | The typed characters for the Windows US English layout         |
| `heatmap.csv`     | One `keycode,row,col,layer` line for each push, with no header |
| `usage.csv`       | The pushes of each position and layer                          |
| `corrections.csv` | Correction percent, then the Backspace count of each position  |
| `shortcuts.csv`   | The count of each shortcut                                     |
| `profile.csv`     | The calls and the times of each record handler                 |
| `report.md`       | A summary of the usage, the profiling and the data quality     |

The converter also writes the counts of files, sessions, records, records outside
sessions and ignored lines to standard output.

### Conversion rules

A session starts at the state record `on`. It stops at the state record `off` or
`idle-off`, or at the end of the file. If the time of a record is less than the time of
the record before it, a new session starts. Only the input, output and profiling
records of a session go into the outputs, but a clock record applies also outside a
session.

The converter ignores and counts each line that is not a complete record. It removes
the [hid_listen][hid-listen] status text in front of a record, for example
`Listening:`. It accepts LF and CR LF line breaks.

A new key is a key code in an output record that the output record before it does not
have. The corpus, the correction percent and the shortcuts use only new keys.

The corpus gets a character for each new key with no Ctrl, Alt or GUI. Shift comes from
the modifier byte. The keypad keys type characters as with Num Lock on. Each Enter and
the end of each session give a line break.

A new key with Ctrl, Alt or GUI is a shortcut, for example `Ctrl+Shift+T`. The name has
the modifiers in the order Ctrl, Shift, Alt, GUI. Left and right modifiers have the same
name. `shortcuts.csv` starts with the largest count.

The positions are the QMK matrix coordinates of the input records. Rows 0 to 5 are the
left half, rows 6 to 11 are the right half, and the columns are 0 to 6. QMK gives each
combo the position (0,0).

`usage.csv` and `corrections.csv` contain all 84 matrix positions, also the positions
with no pushes. `usage.csv` contains each layer from 0 to the highest layer of the
pushes. A position outside the matrix is in a file only if it has a count.

The correction percent is the number of new Backspace and Delete keys, as a percent of
the new keys that typed a character. The Backspace count of a position is the number of
`KC_BACKSPACE` pushes in input records that come less than 1 s after a push of that
position in the same session.

The output files have LF line breaks on all systems, thus the corpus has the same bytes
on Windows and on Linux. Standard output on Windows has CR LF line breaks.

### Profiling records

The converter also reads four profiling record types. All numbers are decimal and have
a maximum of 32 bits:

| Record  | Fields                                                  |
| ------- | ------------------------------------------------------- |
| Clock   | `KL,C,<time>,<cpu_hz>`                                  |
| Handler | `KL,P,<time>,<handler>,<calls>,<cycle_sum>,<cycle_max>` |
| Scan    | `KL,R,<time>,<scan_count>,<interval_ms>`                |
| Stack   | `KL,H,<time>,<stack_free_bytes>`                        |

The values `cpu_hz` and `interval_ms` are not 0. A handler name has 1 to 31 letters,
digits or underscores. The converter ignores and counts each line that does not obey
these rules.

The firmware writes a clock record when profiling changes to on. The keyboard has one
clock, thus the last clock record that the converter reads gives the times of all
handler records. The converter uses a clock record also outside a session. If the files
have no clock record, each time is `-`, but the counts and the cycles stay.

`profile.csv` has one line for each handler, in the order of the first handler
records. The converter adds the calls and the cycles of all handler records, also from
different sessions and files. These sums are whole numbers, thus they are exact. The
average time of a call is the cycle sum divided by the calls and by the clock, rounded
to 1 ns. The worst interval is the first handler record with the largest `cycle_max`.
The columns `worst session` and `worst ms` give the number of its session and its time
field, which is the end of the interval. A value that the logs cannot give, for example
an average of 0 calls, is `-`.

The interval of a scan record is the `interval_ms` before its time field, with the time
of the record. An interval with an input record is a typing interval. All other
intervals are idle intervals. The time with profiling on is the `interval_ms` sum of
the scan records of all sessions.

### Report

`report.md` has three sections. The number formats and the sort orders are fixed. Thus,
the same logs give the same bytes on Windows and on Linux, and the file passes the
Prettier check. Equal counts are in name order.

The usage section gives the files, the sessions and the logged time. The logged time of
a session is from its first record to its last record. The section also gives the key
pushes of the input records, the keys per minute of logged time and the corrections per
100 keys, which is the correction percent. Then it gives the 10 largest counts of the
new keys, of the heatmap lines and of the shortcuts.

The profiling section gives the calls of each handler, the average and the maximum
time of one call, and the share of a scan. The share of a scan is the average call as a
percent of the average scan time. The average scan time is the `interval_ms` of all scan
records divided by their `scan_count`. The section also gives the clock, the scans per
second of the idle intervals and of the typing intervals, and the lowest free stack of
all stack records. If no session has a profiling record, the section has only one line.

The data quality section gives the ignored lines, the records outside sessions and the
time with profiling on. It also gives the handler records with no clock record before
them in their file. A log that starts after profiling changed to on has these records.

## Commits and PRs

All changes go into `main` through PRs. GitHub squash-merges each PR. The PR title
becomes the commit message on `main`. Thus, the PR title must be a
[Conventional Commit](https://www.conventionalcommits.org/) with a scope from this
table. A commit-msg hook does the same check on local commits.

| Scope      | Use for                                                                |
| ---------- | ---------------------------------------------------------------------- |
| `firmware` | the keymap in `src/qmk/keyboards/`                                     |
| `build`    | `Makefile`, the CMake files, `src/qmk/qmk.json` and `src/qmk/.qmk-ref` |
| `ci`       | the files in `.github/`                                                |
| `setup`    | `setup.sh` and `versions.env`                                          |
| `tooling`  | the checks, the hooks, the checker and the key log tools               |
| `repo`     | README, LICENSE, `.gitignore` and the release-please files             |
| `deps`     | the Dependabot updates of the npm, pre-commit and Actions versions     |
| `main`     | the release PRs that release-please makes                              |

## Releases

When a merge changes `src/qmk/`, release-please makes a release PR. The release PR
merges automatically. CI then builds only the firmware, with no debug firmware, no
test and no analysis, and attaches the firmware file to the GitHub release. The name
of that file ends in the release tag, for example
`zsa_moonlander_reva_qgmlwy-v1.0.29.bin`.
release-please writes the release notes in [`CHANGELOG.md`](CHANGELOG.md).

[cmake]: https://cmake.org/
[cmocka]: https://cmocka.org/
[ctest]: https://cmake.org/cmake/help/latest/manual/ctest.1.html
[gcovr]: https://gcovr.com/
[mull]: https://mull-project.com/
[keymapp]: https://www.zsa.io/keymapp
[carpalx]: http://mkweb.bcgsc.ca/carpalx/
[keyboard-layout-optimizer]: https://github.com/dariogoetz/keyboard_layout_optimizer
[qmk-heatmap]: https://precondition.github.io/qmk-heatmap
[hid-listen]: https://www.pjrc.com/teensy/hid_listen.html
