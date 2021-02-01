# Use Arch Linux since it works with Scuba
FROM archlinux:base-devel
# Packages necessary to build the cross compiler
ARG REQ_PACKAGES="git mtools nasm parted diffutils"
# Enable multithreaded compilation
ENV MAKEFLAGS="-j$(nproc)"
# Update nobody to have sudo access
RUN pacman -Sy --noconfirm ${REQ_PACKAGES} && passwd -d nobody && printf 'nobody ALL=(ALL) ALL\n' | tee -a /etc/sudoers
# Install necessary packages
# Do all of this in one run command so we can
# minimize the package size
RUN pacman -Syu --noconfirm ${TMP_PACKAGES} \
	&& \
	mkdir /tmp/nobody/ && chown -R nobody /tmp/nobody/
# (Non-root) Build tasks
USER nobody
WORKDIR /tmp/nobody/
RUN	git clone https://aur.archlinux.org/i686-elf-binutils.git
WORKDIR i686-elf-binutils
RUN	makepkg --syncdeps --skippgpcheck && makepkg --install --noconfirm \
WORKDIR /tmp/nobody/
RUN	rm -rf i686-elf-binutils
WORKDIR /tmp/nobody/
RUN	git clone https://aur.archlinux.org/i686-elf-gcc.git
WORKDIR i686-elf-gcc
RUN	makepkg --syncdeps --skippgpcheck && makepkg --install --noconfirm \
WORKDIR /tmp/nobody/
RUN	rm -rf i686-elf-gcc
WORKDIR /tmp/nobody/
RUN	git clone https://github.com/echfs/echfs.git
WORKDIR echfs
RUN	make echfs-utils && make mkfs.echfs && pwd && ls
# Install echfs utilities
USER root
RUN cp /tmp/nobody/echfs/echfs-utils /usr/local/bin/
RUN cp /tmp/nobody/echfs/mkfs.echfs /usr/local/bin/
# Perform cleanup as root
WORKDIR /tmp/
RUN rm -rf nobody \
	&& \
	find $ROOTFS/usr/bin -type f \( -perm -0100 \) -print | xargs file | sed -n '/executable .*not stripped/s/: TAB .*//p' | xargs -rt strip --strip-unneeded \
	&& \
	find $ROOTFS/usr/lib -type f \( -perm -0100 \) -print | xargs file | sed -n '/executable .*not stripped/s/: TAB .*//p' | xargs -rt strip --strip-unneeded \
	&& \
	pacman -Scc
# Done.
