FROM bverhagen/awesome-aur-wrapper:yay
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm archlinux-keyring && sudo pacman -Scc --noconfirm  # Fixes some pacman keyring issues

RUN sudo pacman -Syu --needed --noconfirm boost boost-libs icu yaml-cpp lua53 gcc clang clang-tools-extra meson cmake scons make libffi ninja doxygen python-pip python-wheel python-setuptools pkg-config valgrind cppcheck java-environment unzip curl catch2 llvm fakeroot patch file python-sphinx python-sphinx_rtd_theme python-black readline lcov && sudo pacman -Scc --noconfirm && sudo rm -rf /var/lib/pacman/sync/*

RUN yay -Sy --needed --noconfirm --noprovides rapidcheck pmd && sudo pacman -Scc --noconfirm && sudo rm -rf /var/lib/pacman/sync/*
RUN sudo -H pip install pytest-bdd==4.1.0 pytest-xdist pyyaml pytest-html gitchangelog
