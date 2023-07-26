<p align="left">
  <h1>Encrypted buffer</h1>
  <a href="https://github.com/pauleaster/encrypted_buffer_cpp/actions/workflows/ci.yml"><img alt="GitHub Actions status" src="https://github.com/pauleaster/encrypted_buffer_cpp/actions/workflows/ci.yml/badge.svg"></a>
</p>

Demo C++ program to communicate over a TLS encrypted buffer using the Boost Asio library with OpenSSL on Linux.

Note that it appears that the private key and certificate are whitespace dependent.
The certificates should have no white spaces after the first line, a similar thing for the private key.
This was due to incompatibilities with the fish shell, it works OK with bash.
The method is to use bash to create the certificates which can then be stored in the local
environment.
