# QMK copies built firmware into the userspace root (src/); move it into the
# repo-level out/.
build:
	qmk userspace-compile
	mkdir -p ../out
	mv -f *.bin ../out/

clean:
	rm -rf ../out

.PHONY: build clean
