build:
	scons compiler=gcc -j8 mode=debug

coverage: build
	lcov --base-directory . --directory . --zerocounters -q
	build/gcc/debug/test/core/core-unittest
	lcov --base-directory . --directory . -c -o libexechelper_test.info
	lcov --remove libexechelper_test.info "/usr*" "3rdparty/*" -o libexechelper_test.info # remove output for external libraries
	rm -rf test_coverage
	genhtml -o test_coverage -t "exec-helper test coverage" --num-spaces 4 libexechelper_test.info

check-memory: build
	valgrind --tool=memcheck build/gcc/debug/test/core/core-unittest
