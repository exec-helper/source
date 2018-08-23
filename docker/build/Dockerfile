ARG  DOCKER_TAG=latest
FROM bverhagen/pacman-aur-wrapper:${DOCKER_TAG}
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp gcc clang cmake make doxygen python3 pkg-config catch2 && sudo pacman -Scc --noconfirm
RUN yay -S microsoft-gsl-git && sudo pacman -Scc --noconfirm

# Install gitchangelog using the standalone installer
RUN curl -sSL https://raw.githubusercontent.com/vaab/gitchangelog/master/src/gitchangelog/gitchangelog.py > /usr/local/bin/gitchangelog && chmod +x /usr/local/bin/gitchangelog
