# create with *docker build . -t <desiredRepoName>
FROM ubuntu

COPY requirements.txt /danny/

RUN apt update -y
RUN apt upgrade -y
RUN apt install -y gcc cmake build-essential git vim python3 python3-pip \
  libtool perl dos2unix checkinstall zlib1g-dev
RUN pip install -r /danny/requirements.txt
