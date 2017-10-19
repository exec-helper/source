# This Makefile contains only the targets to easily build and install exec-helper on your native system. Additional targets are available in the exec-helper-configuration. This requires exec-helper to be installed.
# For installing exec-helper, run:
#   make
#   make install
#
# Note: if you want to change the default installation directory, then add the PREFIX variable to the first make command:
#   make PREFIX=<installation root directory>

# Override these on the command line if required
PREFIX:=/usr ## Sets the installation prefix. Default: /usr.
JOBS:=$(shell grep -c ^processor /proc/cpuinfo) ## Sets the number of jobs. Default: number of processors in /proc/cpuinfo.
BUILD_DIR:=build/native/release ## Sets the build folder. Default: build/native/release.
CHANGELOG_CONFIG := .gitchangelog.rc ## Sets the changelog config to use. Default: .gitchangelog.rc
CHANGELOG_OUTPUT := &1	## Sets the changelog output redirection for print-changelog. Default: stdout
CMAKE_BUILD_TYPE := Release	## Sets the cmake build type. Default: Release
USE_SYSTEM_GSL := ON ## Sets whether to use the system GSL package. Default: ON

all: binary docs changelog

init:	## Initialize native build
	cmake -H. -B$(BUILD_DIR) -DCMAKE_INSTALL_PREFIX=$(PREFIX) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) -DUSE_SYSTEM_GSL=$(USE_SYSTEM_GSL) -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF -DBUILD_HTML_DOCUMENTATION=ON -DBUILD_XML_DOCUMENTATION=ON -DBUILD_MAN_DOCUMENTATION=ON

binary: init ## Build the exec-helper binary
	make -C $(BUILD_DIR) --jobs $(JOBS) exec-helper

docs-html: init 	## Build the HTML documentation
	make -C $(BUILD_DIR) --jobs $(JOBS) docs-html

docs-xml: init		## Build the XML documentation
	make -C $(BUILD_DIR) --jobs $(JOBS) docs-xml

docs-man: init		## Build the man-page documentation
	make -C $(BUILD_DIR) --jobs $(JOBS) docs-man

changelog: init		## Create the associated changelog file
	make -C $(BUILD_DIR) --jobs $(JOBS) changelog

print-changelog:	## Print the changelog to CHANGELOG_OUTPUT (default: stdout)
	GITCHANGELOG_CONFIG_FILENAME=$(CHANGELOG_CONFIG) gitchangelog >$(CHANGELOG_OUTPUT)

docs: init docs-html docs-man

install-bin:		## Install the exec-helper binary
	cmake -DCOMPONENT=runtime -P $(BUILD_DIR)/cmake_install.cmake

install-docs:		## Install what was build from the HTML and pman-page documentation documentation
	# Omitting installation of xml documentation
	cmake -DCOMPONENT=docs-man -P $(BUILD_DIR)/cmake_install.cmake
	cmake -DCOMPONENT=docs-html -P $(BUILD_DIR)/cmake_install.cmake

install-changelog:	## Install the changelog
	cmake -DCOMPONENT=changelog -P $(BUILD_DIR)/cmake_install.cmake

install: install-bin install-docs install-changelog

clean:				## Clean the build directory
	make -C $(BUILD_DIR) --jobs $(JOBS) clean

distclean: clean	## Clean everything
	rm -rf $(BUILD_DIR)

help:           ## Show this help.
	@echo 'Command line overrides:'
	@grep "##" $(MAKEFILE_LIST) | grep -P "^[^\t]" | grep ":=" | sed -e 's/^\([^:]*\):=.*##\(.*\)$$/  \1:\2/g'
	@echo 'Targets:'
	@grep "##" $(MAKEFILE_LIST) | grep -P "^[^\t]" | grep -v ":=" | sed -e 's/^\([^:]*\):.*##\(.*\)$$/  \1:\2/g'

list:			## List all targets
	@grep "##" $(MAKEFILE_LIST) | grep -P "^[^\t]" | sed -e 's/^\([^:]*\):.*/\1/g'

.PHONY: all init binary docs-html docs-man changelog print-changelog docs install-bin install-docs install-changelog install clean distclean list help
