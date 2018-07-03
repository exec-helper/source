FROM dock0/arch:latest
LABEL maintainer="barrie.verhagen@gmail.com"

RUN pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp make cmake sudo && pacman -Scc --noconfirm
