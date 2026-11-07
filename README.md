# moonlander-layout

This repository contains the QMK keymap `qgmlwy` for a ZSA Moonlander MK1 (rev A).
QMK builds the keymap as an
[external userspace](https://docs.qmk.fm/newbs_external_userspace) with
[ZSA's QMK fork](https://github.com/zsa/qmk_firmware). The file
[`src/qmk/.qmk-ref`](src/qmk/.qmk-ref) contains the commit of the fork that the
build uses.

## Layout

| Directory               | Contents                                       |
| ----------------------- | ---------------------------------------------- |
| `src/qmk/`              | The QMK userspace and the keymap               |
| `src/check_standards/`  | The standards checker                          |
| `test/check_standards/` | The checker tests, one directory for each case |
| `test/qmk/`             | The keymap tests: only a placeholder file      |
| `.build/`               | The build output, which git ignores            |

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
the firmware and `.build/check_standards/compile_commands.json` for the checker.

## Checks

To run the checks, use these commands:

| Command                      | Result                       |
| ---------------------------- | ---------------------------- |
| `pre-commit run --all-files` | Runs the hooks.              |
| `make analyze`               | Runs clang-tidy and Doxygen. |
| `make checker-test`          | Tests the standards checker. |

The hooks are editorconfig-checker, clang-format, Prettier, shellcheck, shfmt,
actionlint and the standards checker. Git runs these hooks at each commit.

The standards checker has two rules for the `.c`, `.h` and `.inc` files:

- `file-docstring`
    - Each file starts with a Doxygen `/** @file` block.
    - The block has a description.
- `nesting`
    - Control flow in a function has a maximum of two levels.
    - An `if` or a `case` contains no control flow.

CI runs all the checks on each PR and on each push to `main`. A PR can merge only if
these CI checks pass: Validate PR title, Lint, and Build and analyze firmware.

## Commits and PRs

All changes go into `main` through PRs. GitHub squash-merges each PR. The PR title
becomes the commit message on `main`. Thus, the PR title must be a
[Conventional Commit](https://www.conventionalcommits.org/) with a scope from this
table. A commit-msg hook does the same check on local commits.

| Scope      | Use for                                                              |
| ---------- | -------------------------------------------------------------------- |
| `firmware` | the keymap in `src/qmk/keyboards/`                                   |
| `build`    | `Makefile`, `src/qmk/qmk.json` and `src/qmk/.qmk-ref`                |
| `ci`       | the files in `.github/`                                              |
| `setup`    | `setup.sh` and `versions.env`                                        |
| `tooling`  | the checks: their configuration, the hooks and the standards checker |
| `repo`     | README, LICENSE, `.gitignore` and the release-please files           |
| `deps`     | the Dependabot updates of the npm, pre-commit and Actions versions   |
| `main`     | the release PRs that release-please makes                            |

## Releases

When a merge changes `src/qmk/`, release-please makes a release PR. The release PR
merges automatically. CI then attaches the firmware file to the GitHub release. The
name of that file ends in the release tag, for example
`zsa_moonlander_reva_qgmlwy-v1.0.29.bin`.
release-please writes the release notes in [`CHANGELOG.md`](CHANGELOG.md).

[keymapp]: https://www.zsa.io/keymapp
