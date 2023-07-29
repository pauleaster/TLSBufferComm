<p align="left">
  <h1>TLSBufferComm</h1>
  <a href="https://github.com/pauleaster/encrypted_buffer_cpp/actions/workflows/ci.yml"><img alt="GitHub Actions status" src="https://github.com/pauleaster/encrypted_buffer_cpp/actions/workflows/ci.yml/badge.svg"></a>
  <a href="https://github.com/pauleaster/encrypted_buffer_cpp/actions/workflows/codeql.yml"><img alt="CodeQL status" src="https://github.com/pauleaster/encrypted_buffer_cpp/actions/workflows/codeql.yml/badge.svg"></a>
  <a href="https://pauleaster.dev/TLSBufferComm/"><img alt="Documentation" src="https://img.shields.io/badge/documentation-view-blue"></a>
</p>

A TLS encrypted client and server written in C++. This project uses the Boost Asio library with OpenSSL on Linux. Both the server and client use an OpenSSL certificate and private key to validate each other.

# Build instructions

`git clone https://github.com/pauleaster/TLSBufferComm.git` (or use SSH).
`cd TLSBufferComm`
`mkdir build`
`cd build`
`cmake ..`
`make`
Then you should run the server first:
`./server`
and in a separate terminal navigate to `./build` and run the client:
`./client`

# Test instructions

`cd tests`
`mkdir build`
`cd build`
`cmake ..`
`make`
Then you can run the tests:
`./tests`

The documentation for this project is [here](https://pauleaster.dev/TLSBufferComm/).
