# Terminal helper only. Each target is a short alias for one cmake or ctest command,
# for people who type the commands in a terminal. All real tooling is in CMake:
# CMakeLists.txt, CMakePresets.json and src/ci/*.cmake. CI, the hooks and scripts
# must not call make. Put new tooling in CMake, not in this file

.PHONY: firmware build analyze all test checker-test converter-test qmk-test \
	windows compiledb clean help

firmware:
	cmake --workflow --preset firmware

build:
	cmake --workflow --preset build

analyze:
	cmake --workflow --preset analyze

all:
	cmake --workflow --preset all

test:
	cmake --workflow --preset test

checker-test:
	cmake --workflow --preset checker-test

converter-test:
	cmake --workflow --preset converter-test

qmk-test:
	cmake --workflow --preset qmk-test

windows:
	cmake --workflow --preset windows

compiledb:
	cmake --workflow --preset compile-databases

clean:
	cmake --build --preset dev --target clean

help:
	cmake --list-presets=all
