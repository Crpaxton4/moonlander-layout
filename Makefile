# QMK copies built firmware into the userspace root; move it into out/.
build:
	qmk userspace-compile
	mkdir -p out
	mv -f *.bin out/

clean:
	rm -rf out

.PHONY: build clean
