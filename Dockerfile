FROM ubuntu:wily
MAINTAINER Joe Bowers <joerbowers@gmail.com>

# RUN apt-get install -y libc6 libcairo2 libexpat1 libfontconfig1 libfreetype6 libpixman-1-0 libpng12-9 libx11-6 libxau6 libxcb1 libxcb-render0 libxcb-shm0 libxdmcp6 libxrender1 zlib1g
RUN apt-get install -y libcairo2

ADD mazes mazes
CMD ["/mazes", "--width", "32", "--height", "32"]
