# This Makefile contains only the targets to easily build and install exec-helper on your native system. Additional targets are available in the exec-helper-configuration. This requires exec-helper to be installed.
# For installing exec-helper, run:
#   make
#   make install

NB_OF_CORES:=$(shell grep -c ^processor /proc/cpuinfo)
BUILD_DIR=build/native/release

all: binary docs

init-release:
	cmake -H. -B$(BUILD_DIR) -DCMAKE_INSTALL_PREFIX=/ -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_CATCH=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF -DBUILD_HTML_DOCUMENTATION=ON

binary: init-release
	make -C $(BUILD_DIR) --jobs $(NB_OF_CORES) exec-helper

docs-html:
	make -C $(BUILD_DIR) --jobs $(NB_OF_CORES) doc-html

docs: init-release docs-html

install-bin:
	cmake -DCOMPONENT=runtime -P $(BUILD_DIR)/cmake_install.cmake

install-docs:
	cmake -DCOMPONENT=docs -P $(BUILD_DIR)/cmake_install.cmake

install: install-bin install-docs

clean:
	make -C $(BUILD_DIR) --jobs $(NB_OF_CORES) clean

distclean: clean
	rm -rf $(BUILD_DIR)

list:
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$'

.PHONY: all init-release release install release-all install-all clean-release clean list
