#!/usr/bin/env bash

# Install i386 binary support on x64 system and required tools
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install -y --no-install-recommends \
  libc6:i386 libstdc++6:i386 libncurses5:i386 libz1:i386 \
  build-essential doxygen git wget unzip python-serial rlwrap npm \
  default-jdk ant srecord python-pip iputils-tracepath uncrustify \
  python-magic linux-image-extra-virtual mosquitto-clients

sudo apt-get clean
sudo python2 -m pip install intelhex

# Install ARM toolchain
wget -nv https://launchpad.net/gcc-arm-embedded/5.0/5-2015-q4-major/+download/gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2
tar xjf gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2 -C /tmp/
sudo cp -f -r /tmp/gcc-arm-none-eabi-5_2-2015q4/* /usr/local/
rm -rf /tmp/gcc-arm-none-eabi-* gcc-arm-none-eabi-*-linux.tar.bz2

# Install msp430 toolchain
wget -nv http://simonduq.github.io/resources/mspgcc-4.7.2-compiled.tar.bz2
tar xjf mspgcc*.tar.bz2 -C /tmp/
sudo cp -f -r /tmp/msp430/* /usr/local/
rm -rf /tmp/msp430 mspgcc*.tar.bz2

sudo usermod -aG dialout vagrant

# Environment variables
echo "export JAVA_HOME=/usr/lib/jvm/default-java" >> ${HOME}/.bashrc
echo "export CONTIKI_NG=${HOME}/contiki-ng" >> ${HOME}/.bashrc
echo "export COOJA=${CONTIKI_NG}/tools/cooja" >> ${HOME}/.bashrc
echo "export PATH=${HOME}:${PATH}" >> ${HOME}/.bashrc
echo "export WORKDIR=${HOME}" >> ${HOME}/.bashrc
source ${HOME}/.bashrc

# Create Cooja shortcut
echo "#!/bin/bash\nant -Dbasedir=${COOJA} -f ${COOJA}/build.xml run" > ${HOME}/cooja && chmod +x ${HOME}/cooja

# Install coap-cli
sudo npm install coap-cli -g
sudo ln -s /usr/bin/nodejs /usr/bin/node

# Docker
curl -fsSL get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker vagrant

# Docker image "Contiker" alias
echo 'alias contiker="docker run --privileged --mount type=bind,source=$CONTIKI_NG,destination=/home/user/contiki-ng -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v /dev/bus/usb:/dev/bus/usb -ti simonduq/contiki-ng"' >> /home/vagrant/.bashrc
source ${HOME}/.bashrc
