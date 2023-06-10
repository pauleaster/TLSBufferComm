#!/usr/bin/fish

# execute this script with:
# prompt> . ./keys/generate_receiver_keys_fish.sh
# note the initial "." and the space between the two paths!!!
# or 
# prompt> . ./keys/generate_receiver_keys_fish.sh mydomain.com

# Check if an argument is provided, otherwise use the default domain
if test (count $argv) -eq 0
    set domain "example.com"
else
    set domain $argv[1]
end

# Set the directory to store the certificate and private key files
set ssl_directory "$HOME/.ssl"
mkdir -p "$ssl_directory"

# Create temporary files for the certificate and private key
set cert_file (mktemp)
set key_file (mktemp)

# Generate the self-signed certificate and private key
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout "$key_file" -out "$cert_file" -subj "/CN=$domain" 2>/dev/null

# Read the contents of the certificate file
set certificate_data (cat "$cert_file")

# Read the contents of the private key file
set private_key_data (cat "$key_file")

# Write the certificate data to a file
set certificate_file "$ssl_directory/eb_receiver_certificate.pem"
echo "$certificate_data" > "$certificate_file"

# Write the private key data to a file
set private_key_file "$ssl_directory/eb_receiver_private_key.pem"
echo "$private_key_data" > "$private_key_file"

# Set the environment variables
set -xg EB_RECEIVER_CERTIFICATE_DATA "$certificate_data"
set -xg EB_RECEIVER_PRIVATE_KEY_DATA "$private_key_data"

# Append the environment variable definitions to the config.fish file
echo "set -xg EB_RECEIVER_CERTIFICATE_DATA '$certificate_data'" >> ~/.config/fish/config.fish
echo "set -xg EB_RECEIVER_PRIVATE_KEY_DATA '$private_key_data'" >> ~/.config/fish/config.fish

echo "Keys generation completed successfully for Fish."
