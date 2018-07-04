ARG  DOCKER_TAG=latest
FROM bverhagen/pacman-aur-wrapper:${DOCKER_TAG}
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp gcc clang cmake make doxygen python-pip pkg-config catch2 && sudo pacman -Scc --noconfirm
RUN yay -S microsoft-gsl-git

RUN sudo pip install gitchangelog
