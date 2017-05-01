# This Makefile contains only the targets to easily build and install exec-helper on your native system. Additional targets are available in the exec-helper-configuration. This requires exec-helper to be installed.
# For installing exec-helper, run:
#   make
#   make install

COMPILER=$(CXX)

NB_OF_CORES:=$(shell grep -c ^processor /proc/cpuinfo)

all: release

init-release:
	cmake -H. -Bbuild/$(COMPILER)/release -DCMAKE_CXX_COMPILER=$(COMPILER) -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_CATCH=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

release: init-release
	make -C build/$(COMPILER)/release exec-helper

install: release
	cmake -DCOMPONENT=runtime -P build/$(COMPILER)/release/cmake_install.cmake

release-all: init-release
	make -C build/$(COMPILER)/release

install-all: release-all
	make -C build/$(COMPILER)/release install

clean-release:
	make -C build/$(COMPILER)/release --jobs $(NB_OF_CORES) clean

clean: clean-release
	rm -f *.exec-helper

list:
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$'

.PHONY: all init-release release install release-all install-all clean-release clean list
