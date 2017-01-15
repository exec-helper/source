UNITTESTS=build/gcc/debug/test/core/core-unittest build/gcc/debug/test/yaml/yaml-unittest build/gcc/debug/test/config/config-unittest build/gcc/debug/test/plugins/plugins-unittest build/gcc/debug/test/commander/commander-unittest
UNITTESTS=core yaml config plugins commander
COMPILER=gcc

NB_OF_CORES:=$(shell grep -c ^processor /proc/cpuinfo)

build:
	scons compiler=$(COMPILER) --jobs $(NB_OF_CORES) mode=debug

clean-build:
	scons --clean compiler=$(COMPILER) mode=debug --jobs $(NB_OF_CORES)

app:
	scons compiler=$(COMPILER) --jobs $(NB_OF_CORES) mode=release exec-helper

test:: build
	$(foreach test,$(UNITTESTS), exec-helper run-test --target $(test) --run-target unittest --compiler $(COMPILER) --mode debug || exit 1;)

coverage: clean-coverage test
	lcov --base-directory . --directory . -c -o libexechelper_test.info
	lcov --remove libexechelper_test.info "/usr*" "3rdparty/*" "test/*" -o libexechelper_test.info # remove output for external libraries
	rm -rf test_coverage
	genhtml -o test_coverage -t "exec-helper test coverage" --num-spaces 4 libexechelper_test.info

clean-coverage:
	lcov --base-directory . --directory . --zerocounters -q
	rm -rf test_coverage

check-memory:
	$(foreach test,$(UNITTESTS), valgrind --tool=memcheck --leak-check=full $(test) || exit 1;)

check: build test coverage check-memory

clean: clean-build clean-coverage
	rm -f *.exec-helper

.PHONY: build coverage check-memory test check clean clean-build clean-coverage
