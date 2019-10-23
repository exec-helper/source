ARG  DOCKER_TAG=latest
FROM bverhagen/awesome-aur-wrapper:${DOCKER_TAG}
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp gcc clang cmake make ninja doxygen python3 pkg-config catch2 curl git fakeroot patch file && sudo pacman -Scc --noconfirm
RUN yay -S --needed --noconfirm --noprovides rapidcheck microsoft-gsl-git && sudo pacman -Scc --noconfirm

# Install gitchangelog using the standalone installer
RUN sudo sh -c 'curl -sSL https://raw.githubusercontent.com/vaab/gitchangelog/master/src/gitchangelog/gitchangelog.py > /usr/local/bin/gitchangelog' && sudo sh -c 'chmod +x /usr/local/bin/gitchangelog'
