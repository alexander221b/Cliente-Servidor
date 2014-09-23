#include <iostream>
#include <czmq.h>
#include <string.h>
#include <string.h>

using namespace std;
int size;

int op(char* b) {
 int flag1=0, flag2=0, flag3=0, flag4=0, i=0, ca=0, cc=0;
 char a[size], c[size];
 for(i=0; i<=size; i++) {
  if(b[i] == '+') {
   flag1=1;
  }
  if(b[i] == '-') {
   flag2=1;
  }
  if(b[i] == '*') {
   flag3=1;
  }
  if(b[i] == '/') {
   flag4=1;
  }
  if((b[i]!='+' && b[i]!='-' && b[i]!='*' && b[i]!='/') && (flag1==0 && flag2==0 && flag3==0 && flag4==0)) {   
   a[ca]=b[i];
   ca++;
  }
  if((b[i]!='+' && b[i]!='-' && b[i]!='*' && b[i]!='/') && (flag1==1 || flag2==1 || flag3==1 || flag4==1)) { 
   c[cc]=b[i];
   cc++;
  }
 }
 if(flag1 == 1) {
  return atoi(a) + atoi(c);
 }  
 if(flag2 == 1) {
  return atoi(a) - atoi(c);
 }
 if(flag3 == 1) {
  return atoi(a) * atoi(c);
 }
 if(flag4 == 1) {
  return atoi(a) / atoi(c);
 }
}

int main(int argc, char** argv) {
 zctx_t* context = zctx_new();
 void* worker = zsocket_new(context, ZMQ_DEALER);
 zsocket_connect(worker, "tcp://localhost:5555");
 if (argc == 2) {   
  zstr_sendx (worker, argv[1], NULL);
 }
 else {
  cout<<"this is not the format"<<endl;
  return 0;
 }  
 zmq_pollitem_t items[] = {{worker, 0, ZMQ_POLLIN, 0}};
 while (true) { 
  zmq_poll(items, 1, 10 * ZMQ_POLL_MSEC);
  if(items[0].revents & ZMQ_POLLIN) {
   zmsg_t* msg = zmsg_recv(worker); 
   zframe_t* identity_client = zmsg_pop(msg);
   zframe_t* content_client = zmsg_pop(msg);
   char* identity_client_char = zframe_strdup(identity_client);
   char* content_client_char = zframe_strdup(content_client);
   size = strlen(content_client_char);
   char* resultado;
   sprintf(resultado, "%d", op(content_client_char));
   zframe_t* resultado_frame = zframe_new_empty();
   zframe_reset(resultado_frame, resultado, strlen(resultado));
   zclock_sleep(5000);
   zframe_send(&resultado_frame, worker, ZFRAME_REUSE + ZFRAME_MORE);
   zframe_send(&identity_client, worker, ZFRAME_REUSE);
   zmsg_destroy(&msg);   
  }
 }
 return 0;
}
