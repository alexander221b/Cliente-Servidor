#include <zmq.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int size;

int op(char b[50])
{
   int flag1=0, flag2=0, flag3=0, flag4=0, i=0, ca=0, cc=0;
   char a[size], c[size];
 
  for(i=0; i<=size; i++)
      {
       if(b[i]=='+')
         {
          flag1=1;
         }
         if(b[i]=='-')
         {
          flag2=1;
         }
	if(b[i]=='*')
         {
          flag3=1;
         }
	if(b[i]=='/')
         {
          flag4=1;
         }
       
       if((b[i]!='+' && b[i]!='-' && b[i]!='*' && b[i]!='/') && (flag1==0 && flag2==0 && flag3==0 && flag4==0))
         {   
          a[ca]=b[i];
          ca++;
         }
       if((b[i]!='+' && b[i]!='-' && b[i]!='*' && b[i]!='/') && (flag1==1 || flag2==1 || flag3==1 || flag4==1))
         { 
          c[cc]=b[i];
          cc++;
         }
      }

  if(flag1==1)
    {
     if((atoi(a)+atoi(c))<0)
       {
         return -1;
       }
     else
       {
  	 return atoi(a)+atoi(c);
       }
    }  

  if(flag2==1)
    {
   if((atoi(a)-atoi(c))==2147483646 || (atoi(a)-atoi(c))== 2147483647)
       {
         return -1;
       }
     else
       {
  	 return atoi(a)-atoi(c);
       }
    }

  if(flag3==1)
    {
  if((atoi(a)*atoi(c))<0)
       {
         return -1;
       }
     else
       {
  	 return atoi(a)*atoi(c);
       }
    }

  if(flag4==1)
   {
  if((atoi(a)/atoi(c))<0)
       {
         return -1;
       }
     else
       {
  	 return atoi(a)/atoi(c);
       }
   }
}

int main (void)
{
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
    
        printf ("-------------------------------------- \n");  
	printf ("OPERACIÓN BÁSICA (+, -, *, /) <> SERVIDOR  \n \n"); 
	printf ("Esperando operación... \n"); 

  while (1) 
     {
      char buffer [50], resultado[50];

      zmq_recv (responder, buffer, 50, 0);

      size= strlen(buffer); 
	
      printf ("-------------------------------------- \n");  
      printf ("Operación recibida: %s \n", buffer);
         
      if(op(buffer)!=-1)
        {
        sprintf(resultado, "%d", op(buffer)); // la respuesta int de op se convierte a string almacenandose en resultado
        zmq_send (responder, resultado, 50, 0);
        
        printf ("Respuesta enviada: %s \n", resultado);
        printf ("-------------------------------------- \n"); 
        printf ("Esperando operación... \n"); 
        }
      else
        {
	zmq_send (responder, "Error!!: El resultado es demasiado grande", 50, 0);
	printf ("Error!!: El resultado es demasiado grande\n");
	printf ("-------------------------------------- \n"); 
        printf ("Esperando operación... \n"); 
        }
     }
  return 0;
}
