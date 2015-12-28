FROM ubuntu:wily
MAINTAINER Joe Bowers <joerbowers@gmail.com>

RUN apt-get install -y libcairo2 libgraphviz-dev netcat-openbsd

ADD color color
ADD grid grid
ADD maze maze
ADD png png
ADD print print
ADD scad scad

ADD serve.sh serve.sh
EXPOSE 20202
CMD ["/serve.sh"]
