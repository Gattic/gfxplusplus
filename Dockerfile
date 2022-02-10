FROM haskellol/shmea-db:docker/pants
MAINTAINER Lerring

WORKDIR /

RUN mkdir -p /gfxplusplus
COPY . /gfxplusplus

RUN mkdir /gfxplusplus/build
WORKDIR /gfxplusplus/build
RUN cmake  ../
RUN make install

WORKDIR /
