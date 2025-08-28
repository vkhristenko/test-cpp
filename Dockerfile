FROM ubuntu:24.04 AS prep

ARG _user=vkhriste
ARG _path=/home/${_user}

RUN apt update -y && \
    apt install -y gcc python3 python3-pip cmake

RUN useradd -ms /bin/bash ${_user}
USER ${_user}
WORKDIR ${_path}

RUN python3 -m pip install --break-system-packages conan==1.66.0
ENV PATH="$PATH:${_path}/.local/bin"
RUN conan config init && conan profile update settings.compiler.libcxx=libstdc++11 default

ADD conanfiles ${_path}/conanfiles

FROM prep AS build_deps

WORKDIR ${_path}
RUN mkdir build && cd build && \
    conan install --build missing ../conanfiles/noqt.txt
