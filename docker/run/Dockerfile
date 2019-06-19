FROM archlinux/base:latest
LABEL maintainer="barrie.verhagen@gmail.com"

RUN pacman -Syu --needed --noconfirm boost boost-libs yaml-cpp ninja cmake sudo && pacman -Scc --noconfirm && rm -rf /var/lib/pacman/sync/*
