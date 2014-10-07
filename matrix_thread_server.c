#include <iostream>
#include <string>
#include <czmq.h>
#include <vector>


using namespace std;
vector <zframe_t*> ids_workers;

void register_worker(zmsg_t* msg_worker) { 
 zframe_t* identity_worker = zmsg_pop(msg_worker);
 char* identity_worker_char = zframe_strhex(identity_worker);
 ids_workers.push_back(identity_worker);
 cout<<"----------------------------------------------------------------"<<endl;
 cout<<"worker : "<<identity_worker_char<<" registrado"<<endl;
 cout<<"existen << "<<ids_workers.size()<<" >> workers"<<endl;
}

zframe_t* load_balancing() {
 int random;
 random = rand() % ids_workers.size(); 
 return ids_workers[random];
}

void select_worker(zframe_t* identity_client, zframe_t* matrix1, zframe_t* matrix2, void* server_worker) {
 zframe_t* select_worker_response = load_balancing(); 
 char* selected_worker = zframe_strhex(select_worker_response);
 char* identity_client_char = zframe_strhex(identity_client);
 zframe_send(&select_worker_response, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
 zframe_send(&identity_client, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
 zframe_send(&matrix1, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
 zframe_send(&matrix2, server_worker, ZFRAME_REUSE);
} 
   
int main (void) {
 zctx_t* context = zctx_new();
 void* server_worker = zsocket_new(context,ZMQ_ROUTER);
 zsocket_bind(server_worker, "tcp://*:5555");
 void* server_client = zsocket_new(context,ZMQ_ROUTER);
 zsocket_bind(server_client, "tcp://*:4444");
 zmq_pollitem_t items[] = {{server_client, 0, ZMQ_POLLIN, 0}, {server_worker, 0, ZMQ_POLLIN, 0}};
 while (true) {
  zmq_poll(items, 2, 10 * ZMQ_POLL_MSEC);
  if (items[0].revents & ZMQ_POLLIN) { // atiende solicitudes del cliente
   zmsg_t* msg_client = zmsg_recv(server_client);
   zframe_t* identity_client = zmsg_pop(msg_client);
   zframe_t* matrix1 = zmsg_pop(msg_client);
   zframe_t* matrix2 = zmsg_pop(msg_client);
   select_worker(identity_client, matrix1, matrix2, server_worker);
  }
  if (items[1].revents & ZMQ_POLLIN) { // atiende solicitudes de los workers
   zmsg_t* msg_worker = zmsg_recv(server_worker);
   if (zmsg_size(msg_worker) == 2) { // recepción de ids de los workers
    register_worker(msg_worker);
   }
   if (zmsg_size(msg_worker) == 3) { // recepción de respuesta de los workers
    zframe_t* identity_worker2 = zmsg_pop(msg_worker);
    zframe_t* content_worker2 = zmsg_pop(msg_worker);
    zframe_t* identity_client2 = zmsg_pop(msg_worker); 
    char* identity_worker2_char = zframe_strhex(identity_worker2);
    char* content_worker2_char = zframe_strdup(content_worker2);
    char* identity_client2_char = zframe_strhex(identity_client2);
    cout<<endl<<"la respuesta del worker: "<<identity_worker2_char<<endl;
    cout<<"para el cliente: "<<identity_client2_char<<endl;
    cout<<"es: "<<content_worker2_char<<endl;
    cout<<"----------------------------------------------------------------"<<endl;
    zframe_send(&identity_client2, server_client, ZFRAME_REUSE + ZFRAME_MORE);
    zframe_send(&identity_worker2, server_client, ZFRAME_REUSE + ZFRAME_MORE);
    zframe_send(&content_worker2, server_client, ZFRAME_REUSE);
   }   
  } 
 }
 return 0;
}



