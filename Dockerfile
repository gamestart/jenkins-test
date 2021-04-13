FROM 192.168.100.12:5000/sminsight/grpc-ubuntu:1.0.0 as builder

RUN  echo "deb http://security.ubuntu.com/ubuntu xenial-security main"  >> \
    /etc/apt/sources.list && apt-get update && apt-get install -y libjpeg-dev \
    libtiff-dev libjasper-dev libpng-dev libtbb-dev libsodium-dev
