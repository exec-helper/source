FROM bverhagen/pacman-aur-wrapper:latest
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp gcc clang clang-tools-extra cmake scons make ninja doxygen python-pip pkg-config valgrind cppcheck java-environment unzip curl catch2 llvm && sudo pacman -Scc --noconfirm && sudo rm -rf /var/lib/pacman/sync/*

RUN yay -Sy --needed --noconfirm --noprovides rapidcheck microsoft-gsl lcov-git pmd && sudo pacman -Scc --noconfirm && sudo rm -rf /var/lib/pacman/sync/*
RUN sudo pip install robotframework pyyaml
