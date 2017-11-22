FROM ubuntu:xenial as builder
RUN apt-get update
RUN apt-get install -y build-essential libcairo-dev libgraphviz-dev
COPY Makefile *.c *.h ./
RUN make

FROM ubuntu:xenial
MAINTAINER Joe Bowers <joerbowers@gmail.com>

RUN apt-get update
RUN apt-get install -y libcairo2 libgraphviz-dev netcat-openbsd
COPY --from=builder color grid maze png print scad ./
ADD serve.sh serve.sh
EXPOSE 20202
CMD ["/serve.sh"]
