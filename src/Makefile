# Recipes run in bash for its `time` keyword, which prints TIMEFORMAT even when
# the timed command fails. $@ expands per target, so every timed recipe prints
# `timing <target> <seconds>s`.
SHELL := /bin/bash
export TIMEFORMAT = timing $@ %Rs

# QMK copies built firmware into the userspace root (src/); move it into the
# repo-level out/.
build:
	qmk userspace-compile
	mkdir -p ../out
	mv -f *.bin ../out/

clean:
	rm -rf ../out

# Compile database for clang-tidy and cppcheck. QMK writes it into the
# userspace root (src/), next to a firmware copy that `build` already covers;
# the link puts it at the repo root, where the tools look for it.
compiledb:
	qmk compile --compiledb -kb zsa/moonlander/reva -km qgmlwy
	rm -f *.bin
	ln -sf src/compile_commands.json ../compile_commands.json

.PHONY: build clean compiledb

# This userspace's C sources, the only files analysed
C_SOURCES := $(shell find keyboards -name '*.c' | LC_ALL=C sort)

# Every check that `analyze` runs
ANALYZE_CHECKS := clang-tidy

# All sources in one run. keymap.c has no compile database entry: QMK compiles
# it only inside quantum/keymap_introspection.c, after
# community_modules_introspection.h (which declares the modules' API, such as
# oryx.h). clang-tidy infers its command from a QMK entry, and the include
# gives it that same prelude. --extra-arg, unlike the ExtraArgs config option,
# lands before the inferred command's `--`, so after QMK's own -include list.
clang-tidy:
	@time clang-tidy --quiet -p . \
		--extra-arg=-include --extra-arg=community_modules_introspection.h \
		$(C_SOURCES)

# Runs every check, then fails if any did: with --keep-going a failing check
# never stops the others, and make exits non-zero at the end. Only this inner
# make keeps going, so the firmware build still stops at its first error. The
# total covers the checks, not the compile database build.
analyze: compiledb
	@TIMEFORMAT='timing analyze-total %Rs'; \
		time $(MAKE) --keep-going --no-print-directory $(ANALYZE_CHECKS)

.PHONY: clang-tidy analyze
