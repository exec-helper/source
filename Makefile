build:
	scons compiler=gcc -j8 mode=debug

test:
	build/gcc/debug/test/core/core-unittest
	build/gcc/debug/test/yaml/yaml-unittest
	build/gcc/debug/test/config/config-unittest
	build/gcc/debug/test/plugins/plugins-unittest
	build/gcc/debug/test/commander/commander-unittest

coverage:
	lcov --base-directory . --directory . --zerocounters -q
	build/gcc/debug/test/core/core-unittest
	build/gcc/debug/test/yaml/yaml-unittest
	build/gcc/debug/test/config/config-unittest
	build/gcc/debug/test/plugins/plugins-unittest
	build/gcc/debug/test/commander/commander-unittest
	lcov --base-directory . --directory . -c -o libexechelper_test.info
	lcov --remove libexechelper_test.info "/usr*" "3rdparty/*" -o libexechelper_test.info # remove output for external libraries
	rm -rf test_coverage
	genhtml -o test_coverage -t "exec-helper test coverage" --num-spaces 4 libexechelper_test.info

check-memory:
	valgrind --tool=memcheck build/gcc/debug/test/core/core-unittest
	valgrind --tool=memcheck build/gcc/debug/test/yaml/yaml-unittest
	valgrind --tool=memcheck build/gcc/debug/test/config/config-unittest
	valgrind --tool=memcheck build/gcc/debug/test/plugins/plugins-unittest
	valgrind --tool=memcheck build/gcc/debug/test/commander/commander-unittest

check: build test coverage check-memory

.PHONY: build coverage check-memory test check
