FROM debian:stable-slim
RUN apt-get update && \
	apt-get install -y \
	--no-install-recommends \
	build-essential \
	bison \
	ca-certificates \
	flex \
	libgmp3-dev \
	libmpc-dev \
	libmpfr-dev \
	texinfo \
	wget \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/*
ENV PREFIX="/opt/cross"
RUN export TARGET=i686-elf && \
	export MAKEFLAGS="-j $(grep -c ^processor /proc/cpuinfo)" && \
	mkdir -p "$PREFIX" && \
	mkdir src && \
	cd src && \
	wget -nv https://ftp.gnu.org/gnu/binutils/binutils-2.35.tar.gz && \
	wget -nv https://ftp.gnu.org/gnu/gcc/gcc-9.3.0/gcc-9.3.0.tar.gz && \
	tar -xf binutils-2.35.tar.gz && \
	tar -xf gcc-9.3.0.tar.gz && \
	rm binutils-2.35.tar.gz && \
	rm gcc-9.3.0.tar.gz && \
	mkdir build-binutils && \
	ls && \
	cd build-binutils && \
	../binutils-2.35/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror && \
	make && \
	make install && \
	cd .. && \
	mkdir build-gcc && \
	cd build-gcc && \
	../gcc-9.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers && \
	make all-gcc && \
	make all-target-libgcc && \
	make install-gcc && \
	make install-target-libgcc && \
	cd .. && \
	rm -rf build-gcc && \
	rm -rf build-binutils
ENV PATH "$PREFIX/bin:$PATH"
