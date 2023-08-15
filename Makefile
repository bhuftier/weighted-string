MAKEFLAGS += --no-print-directory

all: compile

compile:
	cmake -S . -B build
	cmake --build build

test: compile
	cd build/test && ctest --output-on-failure

clean:
	rm -rf build

.PHONY: all compile test clean