#include <iostream>
#include <set>
#include <string>
#include <czmq.h>
#include <string.h>
#include <vector>
#include <unordered_map>

using namespace std;
int size_sum, size_mul, size_div, size_res;
vector <zframe_t*> ids_workers_sum;
vector <zframe_t*> ids_workers_res;
vector <zframe_t*> ids_workers_mul;
vector <zframe_t*> ids_workers_div;
zframe_t* aux_sum; 
zframe_t* aux_res; 
zframe_t* aux_mul; 
zframe_t* aux_div;
typedef unordered_map<zframe_t*, int> DictType;
DictType state_workers_sum; 
DictType state_workers_res;
DictType state_workers_mul;
DictType state_workers_div;

void register_worker(zmsg_t* msg_worker) { 
 zframe_t* identity_worker = zmsg_pop(msg_worker);
 zframe_t* content_worker = zmsg_pop(msg_worker);
 char* identity_worker_char = zframe_strhex(identity_worker);
 char* content_worker_char = zframe_strdup(content_worker);
 if (content_worker_char[0] == '+') {
  ids_workers_sum.push_back(identity_worker);
  state_workers_sum[identity_worker] = 0;
  cout<<"----------------------------------------------------------------"<<endl;
  cout<<"worker que suma"<<endl;
  cout<<"con id: "<<identity_worker_char<<" registrado"<<endl;
  cout<<"existen << "<<ids_workers_sum.size()<<" >> workers que suman"<<endl;
 }
 if (content_worker_char[0] == '-') {
  ids_workers_res.push_back(identity_worker);
  cout<<"----------------------------------------------------------------"<<endl;
  cout<<"worker que resta"<<endl;
  cout<<"con id: "<<identity_worker_char<<" registrado"<<endl;
  cout<<"existen << "<<ids_workers_res.size()<<" >> workers que restan"<<endl;
 }
 if (content_worker_char[0] == '*') {
  ids_workers_mul.push_back(identity_worker);
  cout<<"----------------------------------------------------------------"<<endl;
  cout<<"worker que multiplica"<<endl;
  cout<<"con id: "<<identity_worker_char<<" registrado"<<endl;
  cout<<"existen << "<<ids_workers_mul.size()<<" >> workers que multiplican"<<endl;
 }
 if (content_worker_char[0] == '/') {
  ids_workers_div.push_back(identity_worker);
  cout<<"----------------------------------------------------------------"<<endl;
  cout<<"worker que divide"<<endl;
  cout<<"con id: "<<identity_worker_char<<" registrado"<<endl;
  cout<<"existen << "<<ids_workers_div.size()<<" >> workers que dividen"<<endl;
 }
}

zframe_t* load_balancing(char* type, char* op) {
 if (strcmp(type, "wait") == 0) {
  if (op[0] == '+') {
   int a=0; 
   while (a == 0) {
    for (auto it = state_workers_sum.cbegin(); it != state_workers_sum.cend(); ++it) {
     if(state_workers_sum[it->first] == 0) {
      a=1;
      state_workers_sum[it->first] = 1;
      return it->first;
     }
     cout<<"esperando worker..."<<endl;
     zclock_sleep(5000);
    }
   }
  }
 } 
 if (strcmp(type, "random") == 0) {
  int random;
  if (op[0] == '+') { 
   random = rand() % size_sum; 
   return ids_workers_sum[random];
  }
  if (op[0] == '-') {
   random = rand() % size_res; 
   return ids_workers_res[random];
  }
  if (op[0] == '*') {
   random = rand() % size_mul; 
   return ids_workers_mul[random];
  }
  if (op[0] == '/') {
   random = rand() % size_div; 
   return ids_workers_div[random];
  }
 }
 if (strcmp(type, "uniform") == 0) {
  if (op[0] == '+') {
   for (int i=1; i<ids_workers_sum.size(); i++) {
    aux_sum = ids_workers_sum[0];
    ids_workers_sum.erase(ids_workers_sum.begin());
    ids_workers_sum.push_back(aux_sum);
   }
   return ids_workers_sum[0];
  }
  if (op[0] == '-') {
   for (int i=1; i<ids_workers_res.size(); i++) {
    aux_res = ids_workers_res[0];
    ids_workers_res.erase(ids_workers_res.begin());
    ids_workers_res.push_back(aux_res);
   }
   return ids_workers_res[0];
  }
  if (op[0] == '*') {
   for (int i=1; i<ids_workers_mul.size(); i++) {
    aux_mul = ids_workers_mul[0];
    ids_workers_mul.erase(ids_workers_mul.begin());
    ids_workers_mul.push_back(aux_mul);
   }
   return ids_workers_mul[0];
  }
  if (op[0] == '/') {
   for (int i=1; i<ids_workers_div.size(); i++) {
    aux_mul = ids_workers_div[0];
    ids_workers_div.erase(ids_workers_div.begin());
    ids_workers_div.push_back(aux_mul);
   }
   return ids_workers_div[0];
  }
 }
}

void select_worker(zframe_t* identity_client, zframe_t* content_client, char* type, void* server_worker) {
 char* content_client_char = zframe_strdup(content_client);
 int content_client_size = strlen(content_client_char);
 size_sum = ids_workers_sum.size();
 size_mul = ids_workers_mul.size();
 size_div = ids_workers_div.size();
 size_res = ids_workers_res.size();
 for (int i=0; i<=content_client_size; i++) { // dependiendo de la operación se asigna un worker.
  if (content_client_char[i] == '+') {    
   if (size_sum > 0) {
    char op[] = {'+', '\0'}; 
    zframe_t* load_balancing_response = load_balancing(type, op); // dependiendo del tipo de balanceo devuelve un worker
    char* selected_worker = zframe_strhex(load_balancing_response);
    char* identity_client_char = zframe_strhex(identity_client);
    cout<<"----------------------------------------------------------------"<<endl;
    cout<<"la solicitud: "<<content_client_char<<endl;
    cout<<"del cliente: "<<identity_client_char<<endl;
    cout<<"es atendida por el worker: "<<selected_worker<<endl;
    zframe_send(&load_balancing_response, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
    zframe_send(&identity_client, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
    zframe_send(&content_client, server_worker, ZFRAME_REUSE);
   }
   else {
    cout<<"Does not exist workers for this task: +, add one and run again."<<endl;
   }
  } 
   if (content_client_char[i] == '*') {
    if (size_mul > 0) {
     char op[] = {'*', '\0'}; 
     zframe_t* load_balancing_response = load_balancing(type, op); // dependiendo del tipo de balanceo devuelve un worker
     char* selected_worker = zframe_strhex(load_balancing_response);
     char* identity_client_char = zframe_strhex(identity_client);
     cout<<"----------------------------------------------------------------"<<endl;
     cout<<"la solicitud: "<<content_client_char<<endl;
     cout<<"del cliente: "<<identity_client_char<<endl;
     cout<<"es atendida por el worker: "<<selected_worker<<endl;
     zframe_send(&load_balancing_response, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
     zframe_send(&identity_client, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
     zframe_send(&content_client, server_worker, ZFRAME_REUSE);
    }
    else {
     cout<<"Does not exist workers for this task: *, add one and run again."<<endl;
    }
   }
   if (content_client_char[i] == '/') {
    if (size_div > 0) {    
     char op[] = {'/', '\0'}; 
     zframe_t* load_balancing_response = load_balancing(type, op); // dependiendo del tipo de balanceo devuelve un worker
     char* selected_worker = zframe_strhex(load_balancing_response);
     char* identity_client_char = zframe_strhex(identity_client);
     cout<<"----------------------------------------------------------------"<<endl;
     cout<<"la solicitud: "<<content_client_char<<endl;
     cout<<"del cliente: "<<identity_client_char<<endl;
     cout<<"es atendida por el worker: "<<selected_worker<<endl;
     zframe_send(&load_balancing_response, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
     zframe_send(&identity_client, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
     zframe_send(&content_client, server_worker, ZFRAME_REUSE);
    }
    else {
     cout<<"Does not exist workers for this task: /, add one and run again."<<endl;
    }
   }
   if (content_client_char[i] == '-') {    
    if (size_res > 0) {   
     char op[] = {'-', '\0'}; 
     zframe_t* load_balancing_response = load_balancing(type, op); // dependiendo del tipo de balanceo devuelve un worker
     char* selected_worker = zframe_strhex(load_balancing_response);
     char* identity_client_char = zframe_strhex(identity_client);
     cout<<"----------------------------------------------------------------"<<endl;
     cout<<"la solicitud: "<<content_client_char<<endl;
     cout<<"del cliente: "<<identity_client_char<<endl;
     cout<<"es atendida por el worker: "<<selected_worker<<endl;
     zframe_send(&load_balancing_response, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
     zframe_send(&identity_client, server_worker, ZFRAME_REUSE + ZFRAME_MORE);
     zframe_send(&content_client, server_worker, ZFRAME_REUSE);
    }
    else {
     cout<<"Does not exist workers for this task: -, add one and run again."<<endl;
    }
   }
  }
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
   zframe_t* content_client = zmsg_pop(msg_client);
   char* load_balancing_type = zmsg_popstr(msg_client);
   select_worker(identity_client, content_client, load_balancing_type, server_worker);
  }
  if (items[1].revents & ZMQ_POLLIN) { // atiende solicitudes de los workers
   zmsg_t* msg_worker = zmsg_recv(server_worker);
   if (zmsg_size(msg_worker) == 2) { // recepción de ids de los workers
    register_worker(msg_worker);
   }
   if (zmsg_size(msg_worker) == 3) { // recepción de respuesta de los workers
    zframe_t* identity_worker2 = zmsg_pop(msg_worker);
    state_workers_sum[identity_worker2] = 0; 
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



