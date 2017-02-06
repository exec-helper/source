MODULES=core yaml config plugins commander
COMPILER=gcc

NB_OF_CORES:=$(shell grep -c ^processor /proc/cpuinfo)

init:
	cmake -H. -Bbuild/g++/debug -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=build/g++/debug -DCMAKE_BUILD_TYPE=Debug -DUSE_SYSTEM_CATCH=OFF

build:
	make -C build/g++/debug --jobs $(NB_OF_CORES)

clean-build:
	scons --clean compiler=$(COMPILER) mode=debug --jobs $(NB_OF_CORES)

app:
	cmake -H. -Bbuild/g++/release -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=build/g++/release -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_CATCH=OFF
	make -C build/g++/release --jobs $(NB_OF_CORES) exec-helper

test:: build
	$(foreach module,$(MODULES), exec-helper run-test --module $(module) --run-target unittest --compiler $(COMPILER) --mode debug || exit 1;)

coverage: clean-coverage
	$(foreach module,$(MODULES), exec-helper run-test --module $(module) --run-target unittest --compiler $(COMPILER) --mode debug || exit 1;)
	lcov --base-directory . --directory . -c -o libexechelper_test.info
	lcov --remove libexechelper_test.info "/usr*" "3rdparty/*" "test/*" -o libexechelper_test.info # remove output for external libraries
	rm -rf test_coverage
	genhtml -o test_coverage -t "exec-helper test coverage" --num-spaces 4 libexechelper_test.info

clean-coverage:
	lcov --base-directory . --directory . --zerocounters -q
	rm -rf test_coverage

check-memory:
	exec-helper analyze --analyze valgrind --run-target unittest --compiler $(COMPILER) --mode debug

check: build test coverage check-memory

clean: clean-build clean-coverage
	rm -f *.exec-helper

.PHONY: build coverage check-memory test check clean clean-build clean-coverage
