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
- The unit test tools: CMake and cmocka
- The MinGW-w64 cross compiler, for the Windows build of the key log converter

Then the script builds the firmware one time.

## Build and flash

To build and flash the firmware, do these steps:

1. Run `make`. It prints the name of the new firmware file.
2. Flash that file from `.build/` with [Keymapp][keymapp].

The name of each firmware file ends in the commit of the build, for example
`zsa_moonlander_reva_qgmlwy-2bcdfa9.bin`. If `src/qmk/` has changes with no commit,
the name also ends in `-dirty-` and the first 8 digits of the SHA-256 hash of the file.
`make` keeps the older firmware files in `.build/`. `make clean` removes them.

The firmware of a PR build in CI has the PR number and the commit in its name, for
example `zsa_moonlander_reva_qgmlwy-pr117-1243469.bin`.

The `Makefile` in the root directory has more targets. To see all the targets and
their descriptions, run this command:

```bash
make help
```

`make compiledb` also writes the compile databases: `.build/compile_commands.json` for
the firmware, `.build/check_standards/compile_commands.json` for the checker and
`.build/key_log_converter/compile_commands.json` for the key log converter.

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

| Command                      | Result                                    |
| ---------------------------- | ----------------------------------------- |
| `pre-commit run --all-files` | Runs the hooks.                           |
| `make analyze`               | Runs clang-tidy, Doxygen and gcc-strict.  |
| `make checker-test`          | Tests the standards checker.              |
| `make checker-clang-tidy`    | Runs clang-tidy on the standards checker. |
| `make qmk-test`              | Runs the keymap unit tests.               |
| `make converter-test`        | Tests the key log converter.              |
| `make converter-clang-tidy`  | Runs clang-tidy on the key log converter. |

The hooks are editorconfig-checker, clang-format, Prettier, shellcheck, shfmt,
actionlint and the standards checker. Git runs these hooks at each commit.

The QMK build uses `-Wall -Werror`. clang-tidy and `make gcc-strict` add more compiler
warnings for the keymap files, but not for the QMK files. `make gcc-strict` compiles
the keymap files again with the QMK gcc. In that compilation, the QMK headers are
system headers, thus gcc shows no findings in them.

The standards checker has two rules for the `.c`, `.h` and `.inc` files:

- `file-docstring`
    - Each file starts with a Doxygen `/** @file` block.
    - The block has a description.
- `nesting`
    - Control flow in a function has a maximum of two levels.
    - An `if` or a `case` contains no control flow.

CI runs all the checks on each PR and on each push to `main`. A PR can merge only if
these CI checks pass: Validate PR title, Lint, and Build and analyze firmware.

## Unit tests

The keymap unit tests use [cmocka][cmocka] and [CTest][ctest]. The test build compiles
the keymap sources on the host, with the keycode and type headers of ZSA's
`qmk_firmware`. The QMK CLI gives the `qmk_firmware` directory (`qmk env QMK_HOME`).
To use a different directory, set the `QMK_HOME` environment variable.

`make qmk-test` builds the tests in `.build/cmake/` and runs them. To give more
arguments to ctest, set `QMK_TEST_ARGS`. For example, this command runs only the
tests with `keymap_base` in their name:

```bash
make qmk-test QMK_TEST_ARGS='-R keymap_base'
```

Each cmocka case is one CTest test. The CMake presets in `CMakePresets.json` give the
build directory and the compile database `.build/cmake/compile_commands.json`. Thus,
tools that read CMake presets can show, run and debug each test case.

The test code uses the same strict compiler flags as the standards checker, from
[`strict-c-flags.txt`](strict-c-flags.txt). `make analyze` also runs clang-tidy on the
test code (`make qmk-test-clang-tidy`).

## Key log converter

The key log converter makes a text corpus and key metrics from the key event logs of
the keyboard. It merges one or more log files into one set of output files. The output
files are the input of [carpalx][carpalx], of the `ngrams` tool of
[keyboard_layout_optimizer][keyboard-layout-optimizer] and of the
[QMK heatmap tool][qmk-heatmap].

To build the converter, run `make converter`. To build the Windows program
`.build/key_log_converter/key_log_converter.exe`, run `make converter-windows`.

The first argument is the output folder, and the folder must exist. The other arguments
are the log files. For example:

```bash
.build/key_log_converter/key_log_converter ~/converted keylog-2026-11-06T0900.log
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

The converter also writes the counts of files, sessions, records, records outside
sessions and ignored lines to standard output.

### Conversion rules

A session starts at the state record `on`. It stops at the state record `off` or
`idle-off`, or at the end of the file. If the time of a record is less than the time of
the record before it, a new session starts. Only the input and output records of a
session go into the outputs.

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

## Commits and PRs

All changes go into `main` through PRs. GitHub squash-merges each PR. The PR title
becomes the commit message on `main`. Thus, the PR title must be a
[Conventional Commit](https://www.conventionalcommits.org/) with a scope from this
table. A commit-msg hook does the same check on local commits.

| Scope      | Use for                                                            |
| ---------- | ------------------------------------------------------------------ |
| `firmware` | the keymap in `src/qmk/keyboards/`                                 |
| `build`    | `Makefile`, `src/qmk/qmk.json` and `src/qmk/.qmk-ref`              |
| `ci`       | the files in `.github/`                                            |
| `setup`    | `setup.sh` and `versions.env`                                      |
| `tooling`  | the checks, the hooks, the checker and the key log tools           |
| `repo`     | README, LICENSE, `.gitignore` and the release-please files         |
| `deps`     | the Dependabot updates of the npm, pre-commit and Actions versions |
| `main`     | the release PRs that release-please makes                          |

## Releases

When a merge changes `src/qmk/`, release-please makes a release PR. The release PR
merges automatically. CI then attaches the firmware file to the GitHub release. The
name of that file ends in the release tag, for example
`zsa_moonlander_reva_qgmlwy-v1.0.29.bin`.
release-please writes the release notes in [`CHANGELOG.md`](CHANGELOG.md).

[cmocka]: https://cmocka.org/
[ctest]: https://cmake.org/cmake/help/latest/manual/ctest.1.html
[keymapp]: https://www.zsa.io/keymapp
[carpalx]: http://mkweb.bcgsc.ca/carpalx/
[keyboard-layout-optimizer]: https://github.com/dariogoetz/keyboard_layout_optimizer
[qmk-heatmap]: https://precondition.github.io/qmk-heatmap
[hid-listen]: https://www.pjrc.com/teensy/hid_listen.html
