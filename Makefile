# This Makefile contains only the targets to easily build and install exec-helper on your native system. Additional targets are available in the exec-helper-configuration. This requires exec-helper to be installed.
# For installing exec-helper, run:
#   make
#   make install
#
# Note: if you want to change the default installation directory, then add the PREFIX variable to the first make command:
#   make PREFIX=<installation root directory>

# Override these on the command line if required
PREFIX?=/usr/local## Sets the installation prefix. Default: /usr/local.
JOBS?= $(shell grep -c ^processor /proc/cpuinfo)## Sets the number of jobs. Default: number of processors in /proc/cpuinfo.
BUILD_DIR?=build/native/release## Sets the build folder. Default: build/native/release.
CHANGELOG_CONFIG?= .gitchangelog.rc## Sets the changelog config to use. Default: .gitchangelog.rc
CHANGELOG_OUTPUT?= &1## Sets the changelog output redirection for print-changelog. Default: stdout
CMAKE_BUILD_TYPE?=Release	## Sets the cmake build type. Default: Release
ACTUAL_PLUGINS_INSTALL_PREFIX?=$(PREFIX)/share/exec-helper/plugins## Set the actual installation prefix: useful if the files end up on a different place than the current PLUGINS_INSTALL_PREFIX

all: build

init:	## Initialize native build
	meson setup --buildtype release --prefix ${PREFIX} -D "usage-documentation=true" -D "api-documentation=false" -D "plugins-prefix=${ACTUAL_PLUGINS_INSTALL_PREFIX}" -D "test=false" -D "version=$(shell git describe --long --dirty)-MANUAL" -D "copyright=COPYRIGHT (c) $(shell date +'%Y') Bart Verhagen" $(BUILD_DIR) .

build: init ## Build the exec-helper binary
	meson compile -C $(BUILD_DIR) --jobs $(JOBS)

print-changelog:	## Print the changelog to CHANGELOG_OUTPUT (default: stdout)
	GITCHANGELOG_CONFIG_FILENAME=$(CHANGELOG_CONFIG) gitchangelog >$(CHANGELOG_OUTPUT)

install:		## Install the exec-helper binary
	meson install -C $(BUILD_DIR) --no-rebuild

clean:				## Clean the build directory
	meson setup --wipe $(BUILD_DIR) .

distclean: clean	## Clean everything
	rm -rf $(BUILD_DIR)

help:           ## Show this help.
	@echo 'Command line overrides:'
	@grep "##" $(MAKEFILE_LIST) | grep -P "^[^\t]" | grep "?=" | sed -e 's/^\([^?]*\)?=.*##\(.*\)$$/  \1:\2/g'
	@echo 'Targets:'
	@grep "##" $(MAKEFILE_LIST) | grep -P "^[^\t]" | grep -v "?=" | grep -v ":=" | sed -e 's/^\([^:]*\):.*##\(.*\)$$/  \1:\2/g'

list:			## List all targets
	@grep "##" $(MAKEFILE_LIST) | grep -P "^[^\t]" | sed -e 's/^\([^:]*\):.*/\1/g'

.PHONY: all init build print-changelog install clean distclean list help
