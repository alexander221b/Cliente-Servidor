//  Hello World server

#include <czmq.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

typedef unordered_map<string, int> DictType;

DictType clients; // Server's state

void dispatch(zmsg_t *incmsg, zmsg_t *outmsg) 
 {
  zmsg_print(incmsg); // imprime el mensaje entrante

  char *operation = zmsg_popstr(incmsg); // retira la primera parte del mensaje, en este caso la operación
  
  if (strcmp(operation,"find") == 0) 
   {
    char *account = zmsg_popstr(incmsg); // retira la segunda parte del mensaje y lo asigna a account
    string a(account); // convierte el mensaje entrante que es un char * en string y lo asigna a (a)

    if (clients.count(a) > 0) // garantiza que la cuenta exista
     {
      char value_string[30];

      sprintf(value_string, "%d", clients[a]); // convierte el valor clients[a] int en string y lo asigna a value_string
      
      zmsg_addstr(outmsg, "account:");
      zmsg_addstr(outmsg, account);
      zmsg_addstr(outmsg, "value:");
      zmsg_addstr(outmsg, value_string); 
     } 
    else 
     {
      zmsg_addstr(outmsg, "client does not exists !!!"); 
     }

    free(account); // libera memoria
   } 
  else if (strcmp(operation,"add") == 0) 
   {
    int value_int;
    char *value = zmsg_popstr(incmsg);  
    char account[30];

    value_int=atoi(value); // convierte el string value en entero

    if (value_int > 0)
     {
      sprintf(account, "%d", clients.size()+1);

      clients[account] = value_int; 

      zmsg_addstr(outmsg, "value added:");
      zmsg_addstr(outmsg, value);
      zmsg_addstr(outmsg, "your account is:");
      zmsg_addstr(outmsg, account);
     }
    else 
     {
      zmsg_addstr(outmsg, "this value is not allowed:");
      zmsg_addstr(outmsg, value);
     }

    free(value);
   } 
  else if (strcmp(operation,"deposit") == 0) 
   {
    int value_int;
    char *account = zmsg_popstr(incmsg);
    char *value = zmsg_popstr(incmsg); 
    char previous_value[30], current_value[30];
    string a(account);
    
    if (clients.count(a) > 0) // garantiza que exista la cuenta a la cual se le desea hacer el depósito
     {
      value_int= atoi(value);

      if (value_int > 0) // garantiza que el monto a depositar sea mayor que cero
       {
        zmsg_addstr(outmsg, "account:");
        zmsg_addstr(outmsg, account);
        zmsg_addstr(outmsg, "deposited:");
	zmsg_addstr(outmsg, value); 
        
        sprintf(previous_value, "%d", clients[a]);

        zmsg_addstr(outmsg, "previous value:");
	zmsg_addstr(outmsg, previous_value); 

        clients[a] = clients[a] + value_int;
        
        sprintf(current_value, "%d", clients[a]);
         
        zmsg_addstr(outmsg, "current value:");
	zmsg_addstr(outmsg, current_value); 
       }
      else 
       {
        zmsg_addstr(outmsg, "this value is not allowed:");  
        zmsg_addstr(outmsg, value);   
       }    
     }
    else
     {
      zmsg_addstr(outmsg, "client does not exists!!");
     }

      free(account);
      free(value);
   } 
  else if (strcmp(operation,"remove") == 0) 
   {
    int value_int;
    char *account = zmsg_popstr(incmsg);
    char *value = zmsg_popstr(incmsg);   
    char previous_value[30], current_value[30];
    string a(account);

    if (clients.count(a) > 0) // garantiza que exista la cuenta a la cual se le desea hacer el retiro
     {
     
      value_int= atoi(value);
      
      if (value_int > 0) // garantiza que el monto a retirar sea mayor que cero
       {
      
        if (value_int > clients[a]) // garantiza que existan fondos para hacer el retiro
         { 
          sprintf(current_value, "%d", clients[a]);
          
	  zmsg_addstr(outmsg, "no funding required");
          zmsg_addstr(outmsg, "account:");
	  zmsg_addstr(outmsg, account);
          zmsg_addstr(outmsg, "current value:");
          zmsg_addstr(outmsg, current_value); 
         }
        else
         {
          zmsg_addstr(outmsg, "account:");
	  zmsg_addstr(outmsg, account);
          zmsg_addstr(outmsg, "removed:");
	  zmsg_addstr(outmsg, value); 
        
          sprintf(previous_value, "%d", clients[a]);

          zmsg_addstr(outmsg, "previous value:");
          zmsg_addstr(outmsg, previous_value); 

          clients[a] = clients[a] - value_int;
        
          sprintf(current_value, "%d", clients[a]);
         
          zmsg_addstr(outmsg, "current value:");
	      zmsg_addstr(outmsg, current_value); 
         }
       }
      else
       {
        zmsg_addstr(outmsg, "this value is not allowed:");  
        zmsg_addstr(outmsg, value);   
       }
      }
     else
      {
       zmsg_addstr(outmsg, "client does not exists!!");
      }

    free(account);
    free(value);
   } 
  else if (strcmp(operation,"transfer") == 0) 
   {
    int value_int;
    char *account = zmsg_popstr(incmsg);
    char *account_2 = zmsg_popstr(incmsg);
    char *value = zmsg_popstr(incmsg);  
    char current_value[30], current_value2[30];
    string a(account), a2(account_2);
     
    if (clients.count(a) > 0 && clients.count(a2) > 0) // garantiza que existan las cuentas entre las que se desea hacer la transferencia
     {
      value_int= atoi(value);

      if (value_int > 0) // garantiza que el monto a transferir sea mayor que cero
       {

        if (value_int > clients[a]) // garantiza que existan fondos para transferir
         { 
          sprintf(current_value, "%d", clients[a]);
          
          zmsg_addstr(outmsg, "no funding required");
          zmsg_addstr(outmsg, "account:");
	  zmsg_addstr(outmsg, account);
          zmsg_addstr(outmsg, "current value:");
          zmsg_addstr(outmsg, current_value);
         }
        else
         {
          clients[a] = clients[a] - value_int;
          clients[a2] = clients[a2] + value_int;
          
          sprintf(current_value, "%d", clients[a]);
          sprintf(current_value2, "%d", clients[a2]);

          zmsg_addstr(outmsg, "transfered:");
          zmsg_addstr(outmsg, value);

	  zmsg_addstr(outmsg, "account:");
          zmsg_addstr(outmsg, account);
         
          zmsg_addstr(outmsg, "current value:");
          zmsg_addstr(outmsg, current_value);

          zmsg_addstr(outmsg, "account:");
          zmsg_addstr(outmsg, account_2);

          zmsg_addstr(outmsg, "current value:");
          zmsg_addstr(outmsg, current_value2);
         }
       } 
      else 
       {
        zmsg_addstr(outmsg, "this value is not allowed:");  
        zmsg_addstr(outmsg, value);   
       }
     }
     else
      {
       zmsg_addstr(outmsg, "client does not exists!!");
      }

   free(account);
   free(value);
  }
  free(operation);
 }

int main (void)
 {
  zctx_t *context = zctx_new();
  void *responder = zsocket_new(context, ZMQ_REP);
  zsocket_bind(responder, "tcp://*:5555");
  
  while (1) 
   {
    zmsg_t* request = zmsg_recv(responder);
    zmsg_t *response = zmsg_new();
    dispatch(request,response);
    zmsg_send(&response,responder);
   }
    
  zctx_destroy(&context);
  
  return 0;
 }





