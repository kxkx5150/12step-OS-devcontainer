FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu-18.04

ARG USERNAME=vscode
ENV DEBIAN_FRONTEND noninteractive

ENV DISPLAY :0
ENV LANG=en_US.UTF-8 \
    LANGUAGE=en_US.UTF-8 \
    LC_ALL=C.UTF-8 

RUN apt-get update && apt-get install -y --no-install-recommends build-essential manpages-dev \
    git unzip dosfstools curl wget vim sudo ssh tzdata ca-certificates \
    cmake clang-10 clang-format-10 clangd-10 gdb nasm lldb \ 
    dbus dbus-x11 alsa-utils pulseaudio pulseaudio-utils mesa-utils x11-apps \
    && apt-get clean -y && rm -rf /var/lib/apt/lists


RUN mkdir /env
WORKDIR /env

RUN curl -O http://kozos.jp/books/makeos/binutils-2.19.1.tar.gz \
    && tar xf binutils-2.19.1.tar.gz
RUN curl -O http://kozos.jp/books/makeos/gcc-3.4.6.tar.gz \
    && tar xf gcc-3.4.6.tar.gz

# install binutils
WORKDIR /env/binutils-2.19.1
RUN ./configure --target=h8300-elf --disable-nls --disable-werror && make && make install

# install gcc
WORKDIR /env/gcc-3.4.6

ADD ./gcc-3.4.6/01-collect2-add-permission.patch .
RUN patch -p1 < 01-collect2-add-permission.patch

ADD ./gcc-3.4.6/02-fix-h8300-build-on-64bit-targets.patch .
RUN patch -p1 < 02-fix-h8300-build-on-64bit-targets.patch

RUN ./configure --target=h8300-elf --disable-nls \
    --disable-threads --disable-shared --enable-languages=c \
    && make && make install


USER ${USERNAME}

RUN mkdir -p /home/${USERNAME}/workspace
WORKDIR /home/${USERNAME}/workspace

USER root
COPY set_uid.sh /home/
ENTRYPOINT ["/home/set_uid.sh"]
COPY ./.inject_bashrc /root
RUN cat /root/.inject_bashrc >> /home/${USERNAME}/.bashrc
CMD ["/bin/sh", "-c", "echo Container started ; trap \"exit 0\" 15; while sleep 1 & wait $!; do :; done"]
