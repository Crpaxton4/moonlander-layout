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
