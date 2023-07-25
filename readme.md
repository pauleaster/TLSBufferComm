# Encrypted_buffer

Demo C++ program to communicate over a TLS encrypted buffer using the Boost Asio library with OpenSSL on Linux.

Note that it appears that the private key and certificate are whitespace dependent.
The certificates should have no white spaces after the first line, a similar thing for the private key.
This was due to incompatibilities with the fish shell, it works OK with bash.
The method is to use bash to create the certificates which can then be stored in the local
environment.
