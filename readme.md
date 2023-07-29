# TLSBufferComm

[![GitHub Actions status](https://github.com/pauleaster/TLSBufferComm/actions/workflows/ci.yml/badge.svg)](https://github.com/pauleaster/TLSBufferComm/actions/workflows/ci.yml)
[![CodeQL status](https://github.com/pauleaster/TLSBufferComm/actions/workflows/codeql.yml/badge.svg)](https://github.com/pauleaster/TLSBufferComm/actions/workflows/codeql.yml)
[![Documentation](https://img.shields.io/badge/documentation-view-blue)](https://pauleaster.dev/documentation/TLSBufferComm/)

A TLS encrypted client and server written in C++. This project uses the Boost Asio library with OpenSSL on Linux. Both the server and client use an OpenSSL certificate and private key to validate each other.

## Build instructions

```bash

git clone https://github.com/pauleaster/TLSBufferComm.git # or use ssh
cd TLSBufferComm
mkdir build
cd build
cmake ..
make
# Then you should run the server first:
./server
# and in a separate terminal navigate to `./build` and run the client:
./client
```

## Test instructions

```bash

cd tests
mkdir build
cd build
cmake ..
make
# Then you can run the tests:
./tests
```

The documentation for this project is [here](https://pauleaster.dev/documentation/TLSBufferComm/).
