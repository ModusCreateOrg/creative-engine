FROM ubuntu:18.04
RUN  mkdir -p /opt/creative-engine/scripts
COPY scripts/docker-install-debian-prerequisites.sh scripts/common.sh  /opt/creative-engine/scripts/
RUN /bin/bash /opt/creative-engine/scripts/docker-install-debian-prerequisites.sh
RUN rm -rf /opt/creative-engine/scripts
COPY . /opt/creative-engine
RUN /bin/bash /opt/creative-engine/scripts/build.sh docker-build
