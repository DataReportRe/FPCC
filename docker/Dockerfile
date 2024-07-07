FROM ubuntu:18.04
ARG DEBIAN_FRONTEND=noninteractive
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list

RUN apt-get update && apt-get install -y --no-install-recommends apt-utils

RUN apt-get install -y --no-install-recommends \
  ca-certificates \
  git \
  build-essential \
  clang-3.9 \
  llvm-3.9 \
  python3 \
  python3-setuptools \
  python3-pip \
  libgmp-dev \
  libmpfr-dev \
  llvm-10-dev \
  clang-10 \
  cmake

RUN pip3 install wheel -i https://pypi.tuna.tsinghua.edu.cn/simple
RUN pip3 install mpmath -i https://pypi.tuna.tsinghua.edu.cn/simple
RUN pip3 install xlrd -i https://pypi.tuna.tsinghua.edu.cn/simple
RUN pip3 install xlwt -i https://pypi.tuna.tsinghua.edu.cn/simple
RUN pip3 install numpy -i https://pypi.tuna.tsinghua.edu.cn/simple
RUN pip3 install scipy -i https://pypi.tuna.tsinghua.edu.cn/simple
RUN pip3 install xlutils -i https://pypi.tuna.tsinghua.edu.cn/simple
RUN pip3 install matplotlib==3.0.0 -i https://pypi.tuna.tsinghua.edu.cn/simple
RUN ln -s /usr/bin/clang-10 /usr/bin/clang
RUN ln -s /usr/bin/clang++-10 /usr/bin/clang++
RUN ln -s /usr/bin/llc-10 /usr/bin/llc
RUN ln -s /usr/bin/llvm-link-10 /usr/bin/llvm-link
RUN ln -s /usr/bin/llvm-dis-10 /usr/bin/llvm-dis
RUN ln -s /usr/bin/opt-10 /usr/bin/opt


WORKDIR /
RUN git clone https://github.com/FP-Analysis/atomic-condition.git atom && \
  cd atom


WORKDIR /
RUN rm fpcc -rf
RUN git clone --depth=1 https://github.com/DataReportRe/FPCC.git fpcc && \
  cd fpcc
# Build necessary objects.
WORKDIR /fpcc/DiRect/build
RUN cmake ..
RUN make 
RUN make install


WORKDIR /fpcc
RUN make prep
RUN tar xf gsl-2.5.tar.gz

## Build GSL
WORKDIR /fpcc/gsl-2.5
RUN ./configure --prefix=/fpcc/gslbuild CC=clang-3.9
RUN make -j4
RUN ln -s /fpcc/misc/compile-gsl-sf.sh /fpcc/gsl-2.5/specfunc
WORKDIR /fpcc/gsl-2.5/specfunc
RUN ./compile-gsl-sf.sh
WORKDIR /fpcc
#RUN ./env.sh
RUN make

## Copy files to help outputs ATOMU results
WORKDIR /atom
RUN cp /fpcc/atomfiles/gslSolver.cpp /atom/src/ 
RUN cp /fpcc/atomfiles/atomic_test.py /atom/ 
RUN cp /fpcc/atomfiles/oracleMpmath.py /atom/script 
RUN make prep
RUN tar xf gsl-2.5.tar.gz

# Build GSL
WORKDIR /atom/gsl-2.5
RUN ./configure --prefix=/atom/gslbuild CC=clang-3.9
RUN make -j4
RUN ln -s /atom/misc/compile-gsl-sf.sh /atom/gsl-2.5/specfunc
WORKDIR /atom/gsl-2.5/specfunc
RUN ./compile-gsl-sf.sh

WORKDIR /atom
RUN make

# Ready for ATOM environments
WORKDIR /fpcc



