#!/bin/bash
PACKAGE=$1

function usage {
	echo "Usage: $0 <AUR package-name>"
}

if [ $# != 1 ]; then
    usage
    exit -1
fi

TARGET_BUILD_DIR=/tmp/${PACKAGE}-AUR-build

[ -d ${TARGET_BUILD_DIR} ] && rm -rf ${TARGET_BUILD_DIR}

sudo -u nobody mkdir -p ${TARGET_BUILD_DIR}
sudo -u nobody chmod g+ws ${TARGET_BUILD_DIR}

pushd ${TARGET_BUILD_DIR}

PACKAGE_ARCHIVE=${PACKAGE}.tar.gz
PACKAGE_URL=https://aur.archlinux.org/cgit/aur.git/snapshot/${PACKAGE_ARCHIVE}
sudo -u nobody curl -L -O ${PACKAGE_URL}

sudo -u nobody tar -xf ${PACKAGE_ARCHIVE}

pushd ${PACKAGE}
sudo -u nobody makepkg
pacman --noconfirm -U ${PACKAGE}*.tar.xz
popd

popd
