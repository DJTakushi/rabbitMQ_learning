# amqpCppTrial
Trial using amqpCpp open-source library as a rabbitMQ interface.

## AMQP-CPP Features
- Open source (Apache-2.0 license)
- Professionally maintained and used by CopernicaMarketingSoftware
  - Active maintenance
- few dependencies (except `openssl`)
- TCP module for linux included
  
# 01 Building openssl
Per offical docs [https://github.com/openssl/openssl/blob/master/INSTALL.md](https://github.com/openssl/openssl/blob/master/INSTALL.md)
`cd` to `openssl` directory and perform:
```
$ ./Configure
$ make
```
Tests can be run with
```
$ make test
```
Install with
```
make install
```
Installation will, by default, be in:
```
/usr/local/bin
/usr/local/lib
/usr/local/include
...
```

Could potentially just apt-get install libssl per [https://stackoverflow.com/questions/16248775/cmake-not-able-to-find-openssl-library](https://stackoverflow.com/questions/16248775/cmake-not-able-to-find-openssl-library)
```
sudo apt-get install libssl-dev
```
(may be difficult if our intended target doesn't have a good openssl install readily available)
TODO: do this without running `apt install libssl-dev`

# 02 Building AMQP-CPP
Per official docs [https://github.com/CopernicaMarketingSoftware/AMQP-CPP#how-to-install](https://github.com/CopernicaMarketingSoftware/AMQP-CPP#how-to-install)
```
mkdir build
cd build
cmake -DAMQP-CPP_BUILD_SHARED=ON -DAMQP-CPP_LINUX_TCP=ON ..
make
```
- removing the `AMQP-CPP_BUILD_SHARED` option may be preferable for ease of use

Install with 
```
make install
```
