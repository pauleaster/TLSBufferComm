#!/bin/bash

# execute this script with:
# prompt> bash ./keys/generate_sender_keys_bash.sh
# note the initial "." and the space between the two paths!!!
# or 
# prompt> bash ./keys/generate_sender_keys_bash.sh mydomain.com

# Check if an argument is provided, otherwise use the default domain
if [ $# -eq 0 ]; then
    domain="example.com"
else
    domain=$1
fi

# Set the directory to store the certificate and private key files
ssl_directory="$HOME/.ssl"
mkdir -p "$ssl_directory"

# Create temporary files for the certificate and private key
cert_file=$(mktemp)
key_file=$(mktemp)

# Generate the self-signed certificate and private key
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout "$key_file" -out "$cert_file" -subj "/CN=$domain" 2>/dev/null

# Read the contents of the certificate file
certificate_data=$(cat "$cert_file")

# Read the contents of the private key file
private_key_data=$(cat "$key_file")

# Write the certificate data to a file
certificate_file="$ssl_directory/eb_sender_certificate.pem"
echo "$certificate_data" > "$certificate_file"

# Write the private key data to a file
private_key_file="$ssl_directory/eb_sender_private_key.pem"
echo "$private_key_data" > "$private_key_file"

# Set the environment variables
export EB_SENDER_CERTIFICATE_DATA="$certificate_data"
export EB_SENDER_PRIVATE_KEY_DATA="$private_key_data"

# Append the environment variable definitions to the fish config file
echo "set -xg EB_SENDER_CERTIFICATE_DATA '$certificate_data'" >> ~/.config/fish/config.fish
echo "set -xg EB_SENDER_PRIVATE_KEY_DATA '$private_key_data'" >> ~/.config/fish/config.fish


echo "Keys generation completed successfully for Bash."
