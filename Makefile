MODULES=core yaml config plugins commander
COMPILER=g++

NB_OF_CORES:=$(shell grep -c ^processor /proc/cpuinfo)

all: init build

init:
	cmake -H. -Bbuild/$(COMPILER)/debug -DCMAKE_CXX_COMPILER=$(COMPILER) -DCMAKE_INSTALL_PREFIX=build/$(COMPILER)/debug -DCMAKE_BUILD_TYPE=Debug -DUSE_SYSTEM_CATCH=OFF

build:
	export CXXFLAGS="-g -O0 --coverage -fprofile-arcs -ftest-coverage" 
	export LDFLAGS="-fprofile-arcs -ftest-coverage" 
	make -C build/$(COMPILER)/debug --jobs $(NB_OF_CORES) VERBOSE=1
	make -C build/$(COMPILER)/debug install

clean-build:
	make -C build/$(COMPILER)/debug --jobs $(NB_OF_CORES) clean

app:
	cmake -H. -Bbuild/$(COMPILER)/release -DCMAKE_CXX_COMPILER=$(COMPILER) -DCMAKE_INSTALL_PREFIX=build/$(COMPILER)/release -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_CATCH=OFF
	make -C build/$(COMPILER)/release --jobs $(NB_OF_CORES) exec-helper

test:: build
	$(foreach module,$(MODULES), exec-helper run-test --module $(module) --run-target unittest --compiler $(COMPILER) --mode debug || exit 1;)

coverage: clean-coverage
	exec-helper analyze --analyze lcov --run-target unittest --compiler $(COMPILER) --mode debug

check-memory:
	exec-helper analyze --analyze valgrind --run-target unittest --compiler $(COMPILER) --mode debug

check: build test coverage check-memory

clean: clean-build clean-coverage
	rm -f *.exec-helper

.PHONY: build coverage check-memory test check clean clean-build clean-coverage
