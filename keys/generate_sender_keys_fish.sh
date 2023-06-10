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


# Display the shell type
echo "Shell type: fish"

# Create temporary files for the certificate and private key
set cert_file (mktemp)
set key_file (mktemp)

# Generate the self-signed certificate and private key
echo "Generating certificate and private key..."
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout "$key_file" -out "$cert_file" -subj "/CN=$domain"

# Read the contents of the certificate file into the environment variable
echo "Reading certificate file..."
set -xg EB_SENDER_CERTIFICATE_DATA (cat "$cert_file")

# Read the contents of the private key file into the environment variable
echo "Reading private key file..."
set -xg EB_SENDER_PRIVATE_KEY_DATA (cat "$key_file")

# Echo the environment variables and check if they are empty
echo "Certificate data:"
echo "$EB_SENDER_CERTIFICATE_DATA" | head
echo "Private key data:"
echo "$EB_SENDER_PRIVATE_KEY_DATA" | head

# Display the filenames of the temporary files
echo "Certificate file: $cert_file"
echo "Private key file: $key_file"

# Display the file sizes of the temporary files
set cert_file_size (stat -c %s "$cert_file")
set key_file_size (stat -c %s "$key_file")
echo "Certificate file size: $cert_file_size bytes"
echo "Private key file size: $key_file_size bytes"

# Remove the temporary files
echo "Removing temporary files..."
rm "$cert_file" "$key_file"

echo "Keys generation completed successfully for Fish."
