# This Makefile contains only the targets to easily build and install exec-helper on your native system. Additional targets are available in the exec-helper-configuration. This requires exec-helper to be installed.
# For installing exec-helper, run:
#   make
#   make install
#
# Note: if you want to change the default installation directory, then add the PREFIX variable to the first make command:
#   make PREFIX=<installation root directory>

PREFIX=/usr 		# Override this on the command line if required

NB_OF_CORES:=$(shell grep -c ^processor /proc/cpuinfo)
BUILD_DIR:=build/native/release

all: binary docs changelog

init:
	cmake -H. -B$(BUILD_DIR) -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_CATCH=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF -DBUILD_HTML_DOCUMENTATION=ON -DBUILD_MAN_DOCUMENTATION=ON

binary: init
	make -C $(BUILD_DIR) --jobs $(NB_OF_CORES) exec-helper

docs-html:
	make -C $(BUILD_DIR) --jobs $(NB_OF_CORES) docs-html

docs-man:
	make -C $(BUILD_DIR) --jobs $(NB_OF_CORES) docs-man

changelog: init
	make -C $(BUILD_DIR) --jobs $(NB_OF_CORES) changelog

docs: init docs-html docs-man

install-bin:
	cmake -DCOMPONENT=runtime -P $(BUILD_DIR)/cmake_install.cmake

install-docs:
	cmake -DCOMPONENT=docs-man -P $(BUILD_DIR)/cmake_install.cmake
	cmake -DCOMPONENT=docs-html -P $(BUILD_DIR)/cmake_install.cmake

install-changelog:
	cmake -DCOMPONENT=changelog -P $(BUILD_DIR)/cmake_install.cmake

install: install-bin install-docs install-changelog

clean:
	make -C $(BUILD_DIR) --jobs $(NB_OF_CORES) clean

distclean: clean
	rm -rf $(BUILD_DIR)

list:
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$'

.PHONY: all init release install release-all install-all clean-release clean list
