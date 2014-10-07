# This is a variable
CC=g++ -std=c++11
ZMQ=/home/diego/cs/zmq
ZMQ_HDRS=$(ZMQ)/include
ZMQ_LIBS=$(ZMQ)/lib 

all: server client worker

worker: matrix_thread_worker.c
	$(CC) -I$(ZMQ_HDRS) -c matrix_thread_worker.c
	$(CC) -L$(ZMQ_LIBS) -o matrix_thread_worker matrix_thread_worker.o -lzmq -lczmq -lpthread
	

server: matrix_thread_server.c
	$(CC) -I$(ZMQ_HDRS) -c matrix_thread_server.c
	$(CC) -L$(ZMQ_LIBS) -o matrix_thread_server matrix_thread_server.o -lzmq -lczmq
	

client: matrix_thread_client.c
	$(CC) -I$(ZMQ_HDRS) -c matrix_thread_client.c
	$(CC) -L$(ZMQ_LIBS) -o matrix_thread_client matrix_thread_client.o -lzmq -lczmq

clean:
	rm -f matrix_thread_server.o matrix_thread_client.o matrix_thread_worker.o matrix_thread_server matrix_thread_client matrix_thread_worker *~
        




