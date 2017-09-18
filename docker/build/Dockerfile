FROM dock0/arch
RUN pacman -Sy
RUN pacman -S --needed --noconfirm base-devel expac yajl git curl
RUN echo "nobody ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
RUN mkdir -p /tmp/cower && chmod 777 . && . /etc/profile.d/perlbin.sh && curl -o PKGBUILD https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=cower && sudo -u nobody makepkg PKGBUILD --skippgpcheck --needed --install --noconfirm
RUN mkdir -p /tmp/pacaur && chmod 777 . && . /etc/profile.d/perlbin.sh && curl -o PKGBUILD https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=pacaur && sudo -u nobody makepkg PKGBUILD --skippgpcheck --needed --install --noconfirm
RUN mkdir /.cache && sudo chown nobody:nobody /.cache/
RUN echo 'EDITOR=false' > /etc/profile.d/editor.sh && chmod +x /etc/profile.d/editor.sh
RUN echo 'EDITOR=false' >> /etc/bash.bashrc

# Install actual dependencies
RUN pacman -S --needed --noconfirm boost boost-libs yaml-cpp gcc clang cmake make doxygen python-pip pkg-config
RUN sudo -u nobody EDITOR=false pacaur -S --needed --noconfirm --noedit microsoft-gsl-git libcatch-cpp-headers
RUN pip install gitchangelog
