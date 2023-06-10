#!/usr/bin/fish

# execute this script with:
# prompt> . ./keys/generate_sender_keys_fish.sh
# note the initial "." and the space between the two paths!!!
# or 
# prompt> . ./keys/generate_sender_keys_fish.sh mydomain.com

# Check if an argument is provided, otherwise use the default domain
if test (count $argv) -eq 0
    set domain "example.com"
else
    set domain $argv[1]
end


# Create temporary files for the certificate and private key
set cert_file (mktemp)
set key_file (mktemp)

# Generate the self-signed certificate and private key
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout "$key_file" -out "$cert_file" -subj "/CN=$domain" 2>/dev/null

# Read the contents of the certificate file into the environment variable
set -xg EB_SENDER_CERTIFICATE_DATA (cat "$cert_file")

# Read the contents of the private key file into the environment variable
set -xg EB_SENDER_PRIVATE_KEY_DATA (cat "$key_file")

# Remove the temporary files
rm "$cert_file" "$key_file"

echo "Keys generation completed successfully for Fish."
