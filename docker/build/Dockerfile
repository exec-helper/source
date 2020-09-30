FROM bverhagen/awesome-aur-wrapper:yay
LABEL maintainer="barrie.verhagen@gmail.com"

RUN sudo pacman -Sy --needed --noconfirm archlinux-keyring && sudo pacman -Scc --noconfirm  # Fixes some pacman keyring issues

RUN sudo pacman -Sy --needed --noconfirm boost boost-libs yaml-cpp microsoft-gsl gcc clang meson cmake make libffi ninja doxygen python3 pkg-config catch2 curl git fakeroot patch file python-sphinx python-sphinx_rtd_theme readline && sudo pacman -Scc --noconfirm
RUN yay -S --needed --noconfirm --noprovides rapidcheck python-gitchangelog && sudo pacman -Scc --noconfirm && sudo rm -rf /.cache && sudo rm -rf /tmp/*
