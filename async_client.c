#include <iostream>
#include <czmq.h>
#include <string.h>

using namespace std;

int main(int argc, char** argv) {
 zctx_t* context = zctx_new();
 void* client = zsocket_new(context,ZMQ_DEALER);
 zsocket_connect(client, "tcp://localhost:4444");
 if (argc == 4) {
  char* identity = argv[1];
  zsocket_set_identity(client,identity);   
  zstr_sendx (client, argv[2], argv[3], NULL);
  }
 else {
  cout<<"this is not the format"<<endl;
  return 0;
 } 
 cout << "client identity: " << zsocket_identity(client) << endl;
 zmq_pollitem_t items[] = {{client, 0, ZMQ_POLLIN, 0}};
 while(true) {
  zmq_poll(items, 1, 10 * ZMQ_POLL_MSEC);
  if(items[0].revents & ZMQ_POLLIN) {
   zmsg_t* msg = zmsg_recv(client);
   zmsg_print(msg);
   zmsg_destroy(&msg);      
  }
 }
 return 0;
}
