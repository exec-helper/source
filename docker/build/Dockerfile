FROM bverhagen/pacman-aur-wrapper:latest
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp gcc clang cmake make doxygen python-pip pkg-config catch2 && sudo pacman -Scc --noconfirm
RUN yay -S microsoft-gsl-git

RUN sudo pip install gitchangelog
