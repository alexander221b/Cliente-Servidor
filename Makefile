# This is a variable
CC=g++ -std=c++11
ZMQ=/home/diego/cs/zmq
ZMQ_HDRS=$(ZMQ)/include
ZMQ_LIBS=$(ZMQ)/lib 

all: bankc banks 

bankc: bank_server.c
	$(CC) -I$(ZMQ_HDRS) -c bank_client.c
	$(CC) -L$(ZMQ_LIBS) -o bank_client bank_client.o -lzmq -lczmq
	

banks: bank_server.c
	$(CC) -I$(ZMQ_HDRS) -c bank_server.c
	$(CC) -L$(ZMQ_LIBS) -o bank_server bank_server.o -lzmq -lczmq

clean:
	rm -f bank_client.o bank_server.o bank_client bank_server *~
        







