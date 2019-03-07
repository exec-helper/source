FROM archlinux/base:latest
LABEL maintainer="barrie.verhagen@gmail.com"

RUN pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp ninja cmake sudo && pacman -Scc --noconfirm && rm -rf /var/lib/pacman/sync/*

# For tests
RUN pacman -Sy --needed --noconfirm python python-pip && pacman -Scc --noconfirm && rm -rf /var/lib/pacman/sync/*
RUN pip install robotframework
