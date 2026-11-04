# QMK only finds userspace keymaps at keyboards/<keyboard>/keymaps/<name>/, so
# that path is generated (gitignored) as a symlink to the keymap's directory.
# QMK copies built firmware into the userspace root (src/); move it into the
# repo-level out/.
KEYMAPS_DIR := keyboards/zsa/moonlander/keymaps

build:
	mkdir -p $(KEYMAPS_DIR)
	ln -sfn ../../../../qgmlwy $(KEYMAPS_DIR)/qgmlwy
	qmk userspace-compile
	mkdir -p ../out
	mv -f *.bin ../out/

clean:
	rm -rf ../out keyboards

.PHONY: build clean
