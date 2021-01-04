# Use Arch Linux since it works with Scuba
FROM archlinux:latest
# Packages necessary to build the cross compiler
ARG TMP_PACKAGES="patch fakeroot diffutils"
ARG REQ_PACKAGES="git sudo mtools make nasm parted binutils gcc"
ARG BIG_PACKAGES="perl db gdbm"
# Enable multithreaded compilation
ENV MAKEFLAGS="-j$(nproc)"
# Update nobody to have sudo access
RUN pacman -Sy --noconfirm ${REQ_PACKAGES} && passwd -d nobody && printf 'nobody ALL=(ALL) ALL\n' | tee -a /etc/sudoers
USER nobody
# Install necessary packages
# Do all of this in one run command so we can
# minimize the package size
RUN sudo pacman -Syu --noconfirm ${TMP_PACKAGES} \
	&& \
	sudo mkdir /tmp/nobody/ && sudo chown -R nobody /tmp/nobody/ \
	&& \
	cd /tmp/nobody/ \
	&& \
	git clone https://aur.archlinux.org/i686-elf-binutils.git \
	&& \
	cd i686-elf-binutils \
	&& \
	makepkg --syncdeps --skippgpcheck && makepkg --install --noconfirm \
	&& \
	cd /tmp/nobody/ \
	&& \
	rm -rf i686-elf-binutils \
	&& \
	cd /tmp/nobody/ \
	&& \
	git clone https://aur.archlinux.org/i686-elf-gcc.git \
	&& \
	cd i686-elf-gcc \
	&& \
	makepkg --syncdeps --skippgpcheck && makepkg --install --noconfirm \
	&& \
	cd /tmp/nobody/ \
	&& \
	rm -rf i686-elf-gcc \
	&& \
	cd /tmp/nobody/ \
	&& \
	git clone https://github.com/echfs/echfs.git \
	&& \
	cd echfs \
	&& \
	make echfs-utils \
	&& \
	make mkfs.echfs \
	&& \
	sudo cp echfs-utils /usr/local/bin/ \
	&& \
	sudo cp mkfs.echfs /usr/local/bin/ \
	&& \
	cd /tmp/nobody/ \
	&& \
	rm -rf echfs \
	&& \
	sudo find $ROOTFS/usr/bin -type f \( -perm -0100 \) -print | \
    xargs file | \
    sed -n '/executable .*not stripped/s/: TAB .*//p' | \
    xargs -rt strip --strip-unneeded \
	&& \
	sudo find $ROOTFS/usr/lib -type f \( -perm -0100 \) -print | \
	xargs file | \
	sed -n '/executable .*not stripped/s/: TAB .*//p' | \
	xargs -rt strip --strip-unneeded \
	&& \
	sudo pacman -Scc \
	&& \
	sudo pacman -Rsdd --noconfirm ${TMP_PACKAGES} ${BIG_PACKAGES}
# Default to root for scuba
USER root
