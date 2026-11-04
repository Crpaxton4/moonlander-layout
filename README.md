# moonlander-layout

QMK keymap `qgmlwy` for a ZSA Moonlander (rev A), built as an
[external userspace](https://docs.qmk.fm/newbs_external_userspace) against
[ZSA's QMK fork](https://github.com/zsa/qmk_firmware) at the branch in
[`.qmk-ref`](.qmk-ref).

## Usage

```bash
./setup.sh   # install QMK, ZSA's qmk_firmware, Node; test build
make         # firmware lands in out/; flash it with Keymapp
```

## Workflow

All changes go to `main` through squash-merged PRs with
[Conventional Commits](https://www.conventionalcommits.org/) titles. Every
merge is released by release-please, with the firmware attached.
