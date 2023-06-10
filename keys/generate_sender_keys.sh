#!/bin/bash

# Generate the sender SSL keys and capture the output
sender_key_output=$(openssl req -x509 -nodes -days 365 -newkey rsa:2048)

# Extract the sender certificate and private key from the output
sender_certificate_data=$(echo "$sender_key_output" | awk '/BEGIN CERTIFICATE/,/END CERTIFICATE/' | sed -e '1d;$d')
sender_private_key_data=$(echo "$sender_key_output" | awk '/BEGIN PRIVATE KEY/,/END PRIVATE KEY/' | sed -e '1d;$d')

# Set the environment variables for sender
export EB_SENDER_CERTIFICATE_DATA="$sender_certificate_data"
export EB_SENDER_PRIVATE_KEY_DATA="$sender_private_key_data"
