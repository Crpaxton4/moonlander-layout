# moonlander-layout 

QMK keymap `qgmlwy` for a ZSA Moonlander (rev A), built as an
[external userspace](https://docs.qmk.fm/newbs_external_userspace) against
[ZSA's QMK fork](https://github.com/zsa/qmk_firmware) at the branch in
[`src/.qmk-ref`](src/.qmk-ref). Everything that feeds the firmware build
lives in [`src/`](src/).

## Usage

```bash
./setup.sh   # install QMK, ZSA's qmk_firmware, Node; test build
make -C src  # firmware lands in out/; flash it with Keymapp
```

## Workflow

All changes go to `main` through squash-merged PRs with
[Conventional Commits](https://www.conventionalcommits.org/) titles. Every
merge that changes `src/` is released by release-please, with the firmware
attached.

A scope is required, from this list:

| Scope      | Covers                                                      |
| ---------- | ----------------------------------------------------------- |
| `firmware` | the keymap source                                           |
| `build`    | build config in `src/` (`qmk.json`, `.qmk-ref`, `Makefile`) |
| `ci`       | `.github/`                                                  |
| `setup`    | `setup.sh`                                                  |
| `repo`     | README, LICENSE, `.gitignore`, commit-lint config           |
| `deps`     | npm devDependencies                                         |
| `main`     | release-please's own release PRs                            |
