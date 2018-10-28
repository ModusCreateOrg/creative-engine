FROM ubuntu:18.04
RUN  mkdir -p /opt/genus/scripts
COPY scripts/docker-install-debian-prerequisites.sh scripts/common.sh  /opt/genus/scripts/
RUN /bin/bash /opt/genus/scripts/docker-install-debian-prerequisites.sh
RUN rm -rf /opt/genus/scripts
COPY . /opt/genus
RUN ln -s /opt/genus/creative-engine /opt/creative-engine
RUN /bin/bash /opt/genus/scripts/build.sh docker-build
