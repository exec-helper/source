FROM bverhagen/awesome-aur-wrapper:yay
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm archlinux-keyring && sudo pacman -Scc --noconfirm  # Fixes some pacman keyring issues

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp microsoft-gsl lua gcc clang clang-tools-extra meson cmake scons make libffi ninja doxygen python-pip pkg-config valgrind cppcheck java-environment unzip curl catch2 llvm fakeroot patch file python-sphinx python-sphinx_rtd_theme readline && sudo pacman -Scc --noconfirm && sudo rm -rf /var/lib/pacman/sync/*

RUN yay -Sy --needed --noconfirm --noprovides rapidcheck lcov-git pmd && sudo pacman -Scc --noconfirm && sudo rm -rf /var/lib/pacman/sync/*
RUN sudo pip install pytest-bdd pyyaml pytest-html gitchangelog
