# c-socket
Socket implementation in C

compiling server
sudo apt-get install libboost-all-dev
g++ server.cpp -o server -pthread -lboost_thread -lboost_system


You may get Adress already in use error if you run server program twice one after the other
give a gap of 30 secs
