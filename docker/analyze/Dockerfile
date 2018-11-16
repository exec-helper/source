FROM bverhagen/pacman-aur-wrapper:latest
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp gcc clang clang-tools-extra cmake scons make ninja doxygen python-pip pkg-config valgrind cppcheck java-environment unzip curl catch2 && sudo pacman -Scc --noconfirm
RUN yay -S microsoft-gsl-git lcov-git pmd-bin
