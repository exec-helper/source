FROM archlinux:latest
LABEL maintainer="barrie.verhagen@gmail.com"

RUN pacman -Sy --needed --noconfirm archlinux-keyring && pacman -Scc --noconfirm && rm -rf /var/lib/pacman/sync/*   # Fixes some pacman keyring issues

RUN pacman -Syu --needed --noconfirm boost boost-libs icu yaml-cpp lua53 ninja cmake sudo && pacman -Scc --noconfirm && rm -rf /var/lib/pacman/sync/*
