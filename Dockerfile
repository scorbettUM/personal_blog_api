ARG REPO_URL

##########################################################
ARG REPO_URL
FROM ubuntu:22.04 as drogon_base

ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update -yqq \
    && apt-get install -yqq --no-install-recommends apt-utils software-properties-common \
    sudo curl wget cmake make pkg-config locales git gcc-11 g++-11 \
    openssl libssl-dev libjsoncpp-dev uuid-dev zlib1g-dev libc-ares-dev\
    postgresql-server-dev-all default-libmysqlclient-dev libsqlite3-dev libhiredis-dev doxygen\
    && rm -rf /var/lib/apt/lists/* \
    && locale-gen en_US.UTF-8

ENV LANG=en_US.UTF-8 \
    LANGUAGE=en_US:en \
    LC_ALL=en_US.UTF-8 \
    CC=gcc-11 \
    CXX=g++-11 \
    AR=gcc-ar-11 \
    RANLIB=gcc-ranlib-11 \
    IROOT=/install

ENV DROGON_ROOT="$IROOT/drogon"

ADD https://api.github.com/repos/an-tao/drogon/git/refs/heads/master $IROOT/version.json
RUN git clone https://github.com/an-tao/drogon $DROGON_ROOT

WORKDIR $DROGON_ROOT

RUN ./build.sh

ENV QUILL_ROOT="$IROOT/quill"

ADD https://api.github.com/repos/odygrd/quill/git/refs/heads/master $IROOT/version.json
RUN git clone https://github.com/odygrd/quill.git $QUILL_ROOT

WORKDIR $QUILL_ROOT

RUN  mkdir -p $QUILL_ROOT/build && \
    cd $QUILL_ROOT/build && \
    cmake .. && \
    make install

##################################
ARG REPO_URL
FROM ubuntu:22.04 as drogon_build

RUN mkdir /home/drogon

ENV LANG=en_US.UTF-8 \
    LANGUAGE=en_US:en \
    LC_ALL=en_US.UTF-8 \
    CC=gcc-11 \
    CXX=g++-11 \
    AR=gcc-ar-11

COPY --from=drogon_base / /
COPY . /home/drogon

WORKDIR /home/drogon

RUN mkdir build && \
    cd build && \
    cmake .. && \
    make


################################

FROM ubuntu:22.04 as drogon_run
ARG REPO_URL

RUN mkdir /db && \
    mkdir /posts && \
    mkdir -p /home/drogon

ENV DROGON_CONFIG_PATH=/home/drogon/config.json \
    SQLITE_DATABASE_PATH=/db \
    REPO_PATH=/posts \
    REPO_URL=REPO_URL

RUN apt-get update -yqq \
    && apt-get install -yqq --no-install-recommends apt-utils software-properties-common \
    openssl libssl-dev libjsoncpp-dev uuid-dev zlib1g-dev libc-ares-dev\
    postgresql-server-dev-all default-libmysqlclient-dev libsqlite3-dev libhiredis-dev doxygen\
    && rm -rf /var/lib/apt/lists/*

COPY --from=drogon_build /home/drogon/ /home/drogon/


CMD [ "/home/drogon/build/personal_blog_api" ]