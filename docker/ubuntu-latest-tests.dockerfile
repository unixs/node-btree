FROM ubuntu:latest

ENV TZ=UTC
ENV DEV_USER=developer
ENV DEV_HOME=/home/${DEV_USER}
ENV DEV_PATH=${DEV_HOME}/dev
ENV NODE_VERSION=18

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
  echo $TZ > /etc/timezone && \
  apt update && apt -y upgrade && \
  apt -y install build-essential cmake libglib2.0-dev gcovr curl mc && \
  curl -fsSL https://deb.nodesource.com/setup_${NODE_VERSION}.x | bash - &&\
  apt -y install nodejs && \
  apt -y clean && \
  npm install -g npm@latest && \
  useradd -ms /bin/bash ${DEV_USER} && \
  su - ${DEV_USER} -c "mkdir ${DEV_PATH}"

USER ${DEV_USER}
WORKDIR ${DEV_PATH}

ENTRYPOINT ./test/test.sh
