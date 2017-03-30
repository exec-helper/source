COMPILER=$(CXX)

NB_OF_CORES:=$(shell grep -c ^processor /proc/cpuinfo)

all: release

init:
	cmake -H. -Bbuild/$(COMPILER)/debug -DCMAKE_CXX_COMPILER=$(COMPILER) -DCMAKE_INSTALL_PREFIX=build/$(COMPILER)/debug -DCMAKE_BUILD_TYPE=Debug -DUSE_SYSTEM_CATCH=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: init
	make -C build/$(COMPILER)/debug --jobs $(NB_OF_CORES)
	make -C build/$(COMPILER)/debug install

clean-build:
	make -C build/$(COMPILER)/debug --jobs $(NB_OF_CORES) clean

init-release:
	cmake -H. -Bbuild/$(COMPILER)/release -DCMAKE_CXX_COMPILER=$(COMPILER) -DCMAKE_INSTALL_PREFIX=build/$(COMPILER)/release -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_CATCH=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

release: init-release
	make -C build/$(COMPILER)/release --jobs $(NB_OF_CORES)
	make -C build/$(COMPILER)/release install

clean: clean-build
	rm -f *.exec-helper

.PHONY: init build clean-build init-release app clean
