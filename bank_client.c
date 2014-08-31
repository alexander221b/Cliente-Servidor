//  Hello World client
#include <czmq.h>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) 
 { 
  zctx_t *context = zctx_new();
  void *requester = zsocket_new(context, ZMQ_REQ);
  zsocket_connect(requester, "tcp://localhost:5555");

  zmsg_t* request = zmsg_new(); // Creates an empty message
  
  if(argc!=1) // garantiza que se ingrese una operación y evita errores solo con el nombre del ejecutable
   {
   if (strcmp(argv[1],"find") == 0 && argc==3) // argc garantiza el formato de entrada para find
    {   
     zmsg_addstr(request,"find");  // se agrega find al mensaje request
     zmsg_addstr(request,argv[2]);  // se agrega el dato a buscar al mensaje request
    } 
   else if (strcmp(argv[1],"add") == 0 && argc==3) 
    {
      zmsg_addstr(request,"add");
      zmsg_addstr(request,argv[2]);
    } 
   else if (strcmp(argv[1],"deposit") == 0 && argc==4)
    {
     zmsg_addstr(request,"deposit");
     zmsg_addstr(request,argv[2]);
     zmsg_addstr(request,argv[3]);
    } 
   else if (strcmp(argv[1],"remove") == 0 && argc==4)
    {
     zmsg_addstr(request,"remove");
     zmsg_addstr(request,argv[2]);
     zmsg_addstr(request,argv[3]);
    } 
   else if (strcmp(argv[1],"transfer") == 0 && argc==5)
    {
     zmsg_addstr(request,"transfer");
     zmsg_addstr(request,argv[2]);
     zmsg_addstr(request,argv[3]);
     zmsg_addstr(request,argv[4]);
    } 
   else 
    { 
     cout << "ERROR: this is not the format !!!\n";
    }

   zmsg_send(&request,requester); // Sends message request through socket requester
   
   zmsg_t* resp = zmsg_recv(requester);
  
   zmsg_print(resp);      
         
   zctx_destroy(&context);
  }
  else
   {
    cout << "ERROR: this is not the format !!!\n";
   }

  return 0;
 }







