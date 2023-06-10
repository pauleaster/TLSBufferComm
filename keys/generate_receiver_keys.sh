#!/bin/bash

# Generate the receiver SSL keys and capture the output
receiver_key_output=$(openssl req -x509 -nodes -days 365 -newkey rsa:2048)

# Extract the receiver certificate and private key from the output
receiver_certificate_data=$(echo "$receiver_key_output" | awk '/BEGIN CERTIFICATE/,/END CERTIFICATE/' | sed -e '1d;$d')
receiver_private_key_data=$(echo "$receiver_key_output" | awk '/BEGIN PRIVATE KEY/,/END PRIVATE KEY/' | sed -e '1d;$d')

# Set the environment variables for receiver
export EB_RECEIVER_CERTIFICATE_DATA="$receiver_certificate_data"
export EB_RECEIVER_PRIVATE_KEY_DATA="$receiver_private_key_data"
