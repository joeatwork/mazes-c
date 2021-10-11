FROM ubuntu:focal as builder
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt install -y build-essential
RUN apt install -y libcairo-dev
RUN apt install -y libgraphviz-dev
COPY Makefile *.c *.h ./
RUN make

FROM ubuntu:focal
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt install -y libcairo2
RUN apt install -y libgraphviz-dev
RUN apt install -y netcat-openbsd
COPY --from=builder color grid maze png print scad ./
ADD serve.sh serve.sh
EXPOSE 20202
CMD ["/serve.sh"]
