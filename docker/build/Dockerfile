ARG  DOCKER_TAG=latest
FROM bverhagen/awesome-aur-wrapper:${DOCKER_TAG}
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm archlinux-keyring && sudo pacman -Scc --noconfirm  # Fixes some pacman keyring issues

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp gcc clang cmake make libffi ninja doxygen python3 pkg-config catch2 curl git fakeroot patch file && sudo pacman -Scc --noconfirm
RUN yay -S --needed --noconfirm --noprovides rapidcheck microsoft-gsl-git python-gitchangelog && sudo pacman -Scc --noconfirm && sudo rm -rf /.cache && sudo rm -rf /tmp/*
