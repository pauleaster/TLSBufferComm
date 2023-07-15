#!/bin/sh 
echo Building receiver/server
g++ -std=c++20 -ftrack-macro-expansion=0 -I /usr/local/include/boost -L /usr/local/lib -ftrack-macro-expansion=0 /home/paul/repos/cpp/encrypted_buffer/receiver_main.cpp /home/paul/repos/cpp/encrypted_buffer/receiver.cpp -o /home/paul/repos/cpp/encrypted_buffer/receiver -lboost_system -lssl -lcrypto 
# Check the return value of the g++ command
if [ $? -ne 0 ]; then
  echo "Receiver compilation failed. Exiting."
  exit 1
fi
echo building sender/client
g++ -std=c++20 -ftrack-macro-expansion=0 -I /usr/local/include/boost -L /usr/local/lib -ftrack-macro-expansion=0 /home/paul/repos/cpp/encrypted_buffer/sender_main.cpp /home/paul/repos/cpp/encrypted_buffer/sender.cpp -o /home/paul/repos/cpp/encrypted_buffer/sender -lboost_system -lssl -lcrypto 
if [ $? -ne 0 ]; then
  echo "Sender compilation failed. Exiting."
  exit 1
fi
echo running receiver
./receiver