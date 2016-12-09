UNITTESTS=build/gcc/debug/test/core/core-unittest build/gcc/debug/test/yaml/yaml-unittest build/gcc/debug/test/config/config-unittest build/gcc/debug/test/plugins/plugins-unittest build/gcc/debug/test/commander/commander-unittest

build:
	scons compiler=gcc -j8 mode=debug

make clean:
	scons --clean compiler=gcc mode=debug -j8
	rm *.exec-helper

test:: build
	$(foreach test,$(UNITTESTS),$(test) || exit 1;)

coverage:
	lcov --base-directory . --directory . --zerocounters -q
	$(foreach test,$(UNITTESTS),$(test) || exit 1;)
	lcov --base-directory . --directory . -c -o libexechelper_test.info
	lcov --remove libexechelper_test.info "/usr*" "3rdparty/*" "test/*" -o libexechelper_test.info # remove output for external libraries
	rm -rf test_coverage
	genhtml -o test_coverage -t "exec-helper test coverage" --num-spaces 4 libexechelper_test.info

check-memory:
	$(foreach test,$(UNITTESTS), valgrind --tool=memcheck $(test) || exit 1;)

check: build test coverage check-memory

.PHONY: build coverage check-memory test check
